import tclab
import numpy as np
import time

lab = tclab.TCLab()
for i in range(300):
    lab.Q1(70)
    lab.Q2(50)
    time.sleep(1)
    print("T1 =", lab.T1)
    print("T2 =", lab.T2)