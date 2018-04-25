import time
import datetime
import sys
import ctypes

class TimestampedStr(str):
    """
    String object, records append operations
    """
    @classmethod
    def __scopy__(cls, timestamps, string, *args, **kwargs):
        instance = cls.__new__(cls, string, *args, **kwargs)
        instance.__timestamps__ = timestamps
        return instance

    def __init__(self, *args, **kwargs):
        timestamp = kwargs.get('timestamp') or datetime.datetime.now()
        if len(self) > 0 and not timestamp:
            raise RuntimeError('Initial timestamp have to be a datetime.')
        self.__timestamps__ = len(self) and timestamp and [(0, timestamp)] or []

    def __iadd__(self, other):
        self.__timestamps__.append((len(self), datetime.datetime.now()))
        call = ctypes.pythonapi.PyBytes_ConcatAndDel
        call.argtypes = [ctypes.POINTER(ctypes.py_object), ctypes.POINTER(ctypes.py_object)]
        call(ctypes.byref(self), other)
        return TimestampedStr.__scopy__(self.__timestamps__, other)

    def splitlines(self, keepends=False, **kwargs):
        splitted = str.splitlines(self, True)
        timestamped, str_pos, raw_str_pos, timestamp_pos = [], 0, 0, 0
        for string in splitted:
            pre_pos, raw_str_pos = raw_str_pos, raw_str_pos + len(string)
            if keepends:
                str_pos = raw_str_pos
            else:
                string = string.rstrip()
                str_pos += len(string)
            timestamp_pos_start = timestamp_pos_end = timestamp_pos_next = timestamp_pos
            timestamp_str_pos, timestamp = self.__timestamps__[timestamp_pos_start]
            while raw_str_pos > timestamp_str_pos:
                if str_pos > timestamp_str_pos:
                    timestamp_pos_end = timestamp_pos_next
                timestamp_pos_next += 1
                if(timestamp_pos_next == len(self.__timestamps__)):
                    break
                timestamp_str_pos, _ = self.__timestamps__[timestamp_pos_next]
            new_timestamps = ([(0, timestamp)] +
                              [(pos - pre_pos, timestamp)
                               for pos, timestamp in
                               self.__timestamps__[timestamp_pos_start + 1:timestamp_pos_end]])
            timestamped.append(TimestampedStr.__scopy__(new_timestamps, string))
            timestamp_pos = timestamp_pos_next
        return timestamped

t0 = time.time()
s1 = TimestampedStr("");
s2 = str("");

s1 += "asd"
s2 += "asd"

print(s1)
print(s2)

import time

t0 = time.time()
for i in range(10000):
    s1 += "%s test\n" % i

for i in range(10000):
    s1 += "%s con-test" % i

for i in range(10000):
    s1 += "%s test\n" % i
t1 = time.time()
total = t1-t0
print("Concat new: %s" % total)

t0 = time.time()
for i in range(10000):
    s2 += "%s test\n" % i

for i in range(10000):
    s2 += "%s con-test" % i

for i in range(10000):
    s2 += "%s test\n" % i

t1 = time.time()
total = t1-t0
print("Concat native: %s" % total)

t0 = time.time()
s1.splitlines
t1 = time.time()
total = t1-t0
print("split new: %s" % total)

t0 = time.time()
s2.splitlines
t1 = time.time()
total = t1-t0
print("split native: %s" % total)
