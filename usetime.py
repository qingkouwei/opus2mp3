#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   usetime.py
@Time    :   2019/09/18 18:38:41
@Author  :   Shen Junwei 
@Version :   1.0
@Contact :   shenjunwei001@ke.com
@License :   (C)Copyright 2018-2028, Qingkouwei
@Desc    :   None
'''
import os
from time import time

t1 = time()
#os.system("./opus2mp3 armnormal.audio audio.mp3")
os.system("./opus2mp3 1574153185044.audio audio.mp3")
#os.system("./opus2mp3 pureopus60s.opus audio.mp3")
t2 = time()
print("Spend time: %s" % (t2 - t1))
