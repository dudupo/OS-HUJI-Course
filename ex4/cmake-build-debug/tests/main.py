
import matplotlib.pyplot as plt


if __name__ == "__main__":


    _file = open("test_plot.txt")

    X, Y = [ ], [ ] 
    for line in _file.readlines():
        x, y = ( _ for _ in  line.split(" "))
        X.append(x)
        Y.append(y)
    

    plt.scatter(X[:-1],Y[:-1])
    plt.show()


