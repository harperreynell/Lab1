import csv
import matplotlib.pyplot as plt
import numpy as np

def read_file():
    res = []
    with open("test_results.csv") as f:
        reader = csv.DictReader(f, delimiter=',')
        for row in reader:
            res.append(row)

    return res

def create_plot(ind, data):
    x = []
    y = []
    for d in data:
        x.append(int(d['threads']))
        y.append(float(d['duration']))

    plt.plot(x, y)
    plt.xlabel("Number of threads")
    plt.ylabel("Execution duration")
    plt.xticks(sorted(set(x)))
    plt.yticks(np.arange(0, max(y) + 0.0005, 0.0005))
    plt.title(f"Test result for {data[0]['rows']}x{data[0]['cols']} matrix")

    plt.savefig(f"{ind}.png")
    plt.close()


if __name__ == '__main__':
    result = read_file()
    for i in range(0, len(result), 9):
        create_plot(min(i, i//9), result[i:i+9])