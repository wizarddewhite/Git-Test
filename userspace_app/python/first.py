#!/usr/bin/python
def buildConnectionString(params):
    """Build a connection string from a dictionary of parameters.
    Returns string."""
    return ";".join(["%s=%s" % (k, v) for k, v in params.items()])

if __name__ == "__main__":
    myParams = {"name":"ShaoHe", \
                "NumberID":"123456" }
    print buildConnectionString(myParams)
    help(buildConnectionString)
