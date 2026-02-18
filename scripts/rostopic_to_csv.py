#!/usr/bin/env python3
import argparse
import csv
import json
import os
import sys
from collections import OrderedDict
from typing import Any

import rclpy
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy
from rclpy.qos import HistoryPolicy
from rclpy.qos import QoSProfile
from rclpy.qos import ReliabilityPolicy
from rclpy.utilities import remove_ros_args
from rosidl_runtime_py.convert import message_to_ordereddict
from rosidl_runtime_py.utilities import get_message


def normalize_topic_type(topic_type: str) -> str:
    value = topic_type.strip()
    if "/msg/" in value:
        return value
    parts = value.split("/")
    if len(parts) == 2:
        return f"{parts[0]}/msg/{parts[1]}"
    raise ValueError(
        f"Invalid topic type '{topic_type}'. Use 'pkg/msg/Type' or 'pkg/Type'."
    )


def to_cell(value: Any) -> Any:
    if isinstance(value, (dict, list, tuple)):
        return json.dumps(value, ensure_ascii=True, separators=(",", ":"))
    return value


def flatten_dict(value: Any, prefix: str, output: "OrderedDict[str, Any]") -> None:
    if isinstance(value, dict):
        for key, item in value.items():
            nested_key = f"{prefix}.{key}" if prefix else str(key)
            flatten_dict(item, nested_key, output)
        return
    output[prefix] = to_cell(value)


class TopicToCsvNode(Node):
    def __init__(
        self,
        topic_name: str,
        topic_type: str,
        output_csv: str,
        queue_size: int,
        best_effort: bool,
    ) -> None:
        super().__init__("rostopic_to_csv")
        self._topic_name = topic_name
        self._topic_type = topic_type
        self._output_csv = output_csv
        self._rows = 0
        self._writer = None
        self._csv_file = None

        output_dir = os.path.dirname(output_csv) or "."
        os.makedirs(output_dir, exist_ok=True)
        self._csv_file = open(output_csv, "w", newline="", encoding="utf-8")

        try:
            msg_type = get_message(normalize_topic_type(topic_type))
        except (AttributeError, ModuleNotFoundError, ValueError) as exc:
            self._csv_file.close()
            raise RuntimeError(
                f"Cannot resolve topic type '{topic_type}': {exc}"
            ) from exc

        qos = QoSProfile(depth=max(1, queue_size))
        qos.history = HistoryPolicy.KEEP_LAST
        qos.durability = DurabilityPolicy.VOLATILE
        qos.reliability = (
            ReliabilityPolicy.BEST_EFFORT
            if best_effort
            else ReliabilityPolicy.RELIABLE
        )

        self._sub = self.create_subscription(msg_type, topic_name, self._callback, qos)
        self.get_logger().info(
            f"Subscribed to {topic_name} ({topic_type}), writing to {output_csv}"
        )

    def _callback(self, msg: Any) -> None:
        row = OrderedDict()
        row["receive_time_ns"] = self.get_clock().now().nanoseconds

        payload = message_to_ordereddict(msg)
        flatten_dict(payload, "", row)

        if self._writer is None:
            fieldnames = list(row.keys())
            self._writer = csv.DictWriter(self._csv_file, fieldnames=fieldnames)
            self._writer.writeheader()

        self._writer.writerow(row)
        self._csv_file.flush()
        self._rows += 1

        if self._rows % 1000 == 0:
            self.get_logger().info(f"Saved {self._rows} rows")

    def close(self) -> None:
        if self._csv_file and not self._csv_file.closed:
            self._csv_file.flush()
            self._csv_file.close()


def build_arg_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Subscribe to a ROS2 topic and save messages to CSV."
    )
    parser.add_argument("--topic", required=True, help="Topic name, e.g. /joint_states")
    parser.add_argument(
        "--type",
        required=True,
        help="Topic type, e.g. sensor_msgs/msg/JointState or sensor_msgs/JointState",
    )
    parser.add_argument("--output", required=True, help="Output CSV path")
    parser.add_argument(
        "--queue-size",
        type=int,
        default=100,
        help="ROS subscription queue size (default: 100)",
    )
    parser.add_argument(
        "--best-effort",
        action="store_true",
        help="Use BEST_EFFORT QoS (default is RELIABLE)",
    )
    return parser


def main() -> int:
    non_ros_args = remove_ros_args(args=sys.argv)
    parser = build_arg_parser()
    args = parser.parse_args(non_ros_args[1:])

    rclpy.init(args=sys.argv)
    node = None
    try:
        node = TopicToCsvNode(
            topic_name=args.topic,
            topic_type=args.type,
            output_csv=args.output,
            queue_size=args.queue_size,
            best_effort=args.best_effort,
        )
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    except Exception as exc:
        print(f"[rostopic_to_csv] {exc}", file=sys.stderr)
        return 1
    finally:
        if node is not None:
            node.close()
            node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
