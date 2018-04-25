import time
from functools import wraps

class hybridmethod(object):
    """
    High performance hybrid function wrapper
    """
    __slot__ = ['context', 'method', 'clsmethod']

    def __init__(self, func):
        self.clsmethod = self.method = wraps(func)(lambda *a, **kw: func(self.context, *a, **kw))

    def classmethod(self, func):
        """
        Function to call when calling with class
        """
        self.clsmethod = wraps(func)(lambda *a, **kw: func(self.context, *a, **kw))
        return self

    def __get__(self, instance, cls):
        if instance is None:
            self.context = cls
            return self.clsmethod
        self.context = instance
        return self.method

class A(object):
    count = 0

    @hybridmethod
    def hc(self):
        self.count += 1

    @hc.classmethod
    def hc(self):
        self.count += 1

    def c(self):
        self.count += 1

start_time = time.time()
a = A()
for i in range(1000000):
    a.hc()
print(a.count)
elapsed_time = time.time() - start_time
print("Hybrid instance call: %s" % elapsed_time)

start_time = time.time()
a = A()
for i in range(1000000):
    a.c()
print(a.count)
elapsed_time = time.time() - start_time
print("Native instance call: %s" % elapsed_time)

start_time = time.time()
for i in range(1000000):
    A.hc()
print(A.count)
elapsed_time = time.time() - start_time
print("Hybrid class call: %s" % elapsed_time)
