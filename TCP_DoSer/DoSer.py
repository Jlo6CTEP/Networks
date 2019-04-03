import argparse
import socket
from multiprocessing import Process, Value
from time import sleep


def ping_ponger(address, port, number):
    print("Thread #{} is set up and running".format(number))
    counter = 0
    while 1:
        counter += 1
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.connect((address, port))
        s.send(int(1).to_bytes(4, byteorder='big'))
        s.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Ping-pong given node to death")
    parser.add_argument("address", type=str, help="target node address")
    parser.add_argument("port", type=int, help="target node port")
    parser.add_argument("thread_count", type=int, help="number of threads")
    parser = parser.parse_args()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    thread_list = []
    for x in range(parser.thread_count):
        thread_list.append(Process(target=ping_ponger, args=(parser.address, parser.port, x)))
        thread_list[x].start()
    sleep(1)
