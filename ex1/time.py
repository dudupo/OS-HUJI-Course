


runningtime = { 
    "runningtime_sys" : {
        "name" : "system call",
        "time" : [ 
            100012,
            10001.6,
            1000.6,
            100.573,
            10.567,
            1.57441 ]
    },
    "runningtime_op" : { 
        "name" : "add opearion",
        "time" : [
            100005,
            10000,
            1000.01,
            100.011,
            10.0087,
            1.00851
            ]
    },
    "runningtime_fun" : { 
        "name" : "function call", 
        "time" : [
            100004,
            10000,
            1000.04,
            100.028,
            10.0274,
            1.02556
            ]
    }
}

from matplotlib import pyplot as plt
if __name__ == "__main__" :
    

    # def plot( measure_obj, axs):        
        # axs.set_xscale("log")
    # plt.show()
# 
    # fig, axs = plt.subplots(3, 1)
    time =  [  10 **(1+i) for i in range(6) ]
    plt.title( "average time of operation relative to iterations ")
    # plt.title( "average time of {0} relative to iterations ".format(measure_obj["name"]) )
    plt.xlabel( "iterations" )
    plt.ylabel( "time / iterations [msec]  (log sacle)" )
    plt.yscale("log")
    legends = [ ]
    for j,  operation in enumerate( runningtime.values() ):
        plt.plot( time, operation["time"])        
        legends.append( operation["name"] )
    plt.legend( legends)
    # plt.show()
    plt.savefig("ex1.png")
