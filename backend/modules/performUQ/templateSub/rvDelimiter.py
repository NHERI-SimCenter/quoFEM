# written: Michael Gardner

# from __future__ import generators

# DO NOT CHANGE THE FACTORY, JUST ADD ADDITIONAL DERIVED CLASSES BELOW WHERE INDICATED
# Polymorhophic factory for creating delimiter handlers
class RvDelimiterFactory:
    factories = {}
    def addFactory(id, delimiterFactory):
        RvDelimiterFactory.factories.put[id] = delimiterFactory
    addFactory = staticmethod(addFactory)
    # A Template Method:
    def createRvDelimiter(id):
        if id not in RvDelimiterFactory.factories:
            RvDelimiterFactory.factories[id] = \
              eval(id + '.Factory()')
        return RvDelimiterFactory.factories[id].create()
    
    createRvDelimiter = staticmethod(createRvDelimiter)

# Abstract base class
class RvDelimiter(object):
    pass

# ADD ADDITIONAL CLASSES HERE FOR DIFFERENT DELIMITERS, REMEMBERING TO ADD FACTORY CLASS WITHIN
# EACH DERIVED CLASS--THIS WILL ENSURE THAT IT IS AUTOMATICALLY ADDED TO FACTORY

# SimCenter-style delimiters
class SimCenterDelimiter(RvDelimiter):
    def replaceRV(self, rv):
        return "\"RV." + rv + "\""

    class Factory:
        def create(self):
            return SimCenterDelimiter()

# UQpy-style delimiters
class UQpyDelimiter(RvDelimiter):
    def replaceRV(self, rv):
        return "<" + rv + ">"

    class Factory:
        def create(self):
            return UQpyDelimiter()
