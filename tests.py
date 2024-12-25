import os
import subprocess as sp
import random

n = int(input("How many tests do you want? "))
pl1 = input("Plyer 1: ")
pl2 = input("Plyer 2: ")
pl3 = input("Plyer 3: ")
pl4 = input("Plyer 4: ")

cnfd = open("default.cnf", "r")
conf = cnfd.read()

dir, _ = os.path.split(__file__)

makep = sp.Popen(["make"], cwd=dir)
makep.wait()


statistics = {}
for i in range(n):
    seed = random.randint(0, 1000)
    args = [f"{dir}/Game", pl1, pl2, pl3, pl4, "-s", f"{seed}"]
    proc = sp.Popen(args, stdin=sp.PIPE, stdout=sp.PIPE, stderr=sp.PIPE)

    out, err = proc.communicate(conf.encode())

    print("Terminated first trial")

    with open(f"out/OUT_{i}.txt", "w") as f:
        f.write(out.decode())

    info = err.decode().splitlines()
    for i in range(len(info)):
        if info[i] == "info: end round 199":
            winer = ["none", 0]
            for j in range(4):
                score = info[i + j + 1].split()[2:]
                if int(score[-1]) > winer[-1]:
                    winer[0] = score[0]
                    winer[-1] = int(score[-1])
            print(winer[0])
            try:
                statistics[winer[0]] += 1
            except KeyError:
                statistics[winer[0]] = 1


for key, value in statistics.items():
    print(f"{key} : {value}")
