import matplotlib.pyplot as plt
import pandas as pd
from collections import defaultdict
import plotly.express as px

# Function to parse the data and organize it into a dictionary
def parse_data(filename):
    strategies = ["FCFS", "Round Robin", "Shortest Job First", "Shortest Remaining Time First", "Multi-level Feedback Queue"]
    data = defaultdict(list)
    data2 = defaultdict(list)
    temp = 0
    temp2 = 0
    with open(filename, "r") as file:
        lines = file.readlines()
        # print(lines)
        i = 0
        while i < len(lines):
            if i%2 == 0:
                strategy = strategies[temp]
                temp += 1
                gantt_chart = lines[i].split()
                if strategy not in data:
                    data[strategy] = {
                        "Gantt Chart":[],
                    }
                data[strategy]["Gantt Chart"].append(gantt_chart)
            else:
                strategy = strategies[temp2]
                temp2 += 1
                metrics = lines[i].split()
                avg_turnaround_time = float(metrics[0])
                avg_response_time = float(metrics[1])
                if strategy not in data2:
                    data2[strategy] = {
                        "Avg Turnaround Time":[],
                        "Avg Response Time":[],
                    }
                data2[strategy]["Avg Turnaround Time"].append(avg_turnaround_time)
                data2[strategy]["Avg Response Time"].append(avg_response_time)
            i += 1
    # print(data)
    # print(data2)
    return data, data2


# Function to plot Gantt chart for a given strategy
def plot_gantt_chart(data):
    for strategy, values in data.items():
        gantt_chart = values["Gantt Chart"]
        gantt_chart = gantt_chart[0]
        # print(gantt_chart)
        chunks = [gantt_chart[i:i+3] for i in range(0, len(gantt_chart), 3)]
        result = [{'jobName': chunk[0], 'startTime': float(chunk[1]), 'endTime': float(chunk[2])} for chunk in chunks]

        df = pd.DataFrame(result, columns=["jobName", "startTime", "endTime"])
        df.sort_values(by='startTime', inplace=True)

        # Create a horizontal bar plot
        fig, ax = plt.subplots(figsize=(10, 6))

        # Plot horizontal bars for each job
        for i, row in df.iterrows():
            ax.barh(row['jobName'], width=row['endTime'] - row['startTime'], left=row['startTime'])

        # Set labels and title
        ax.set_xlabel('Timeline')
        ax.set_ylabel('Jobs')
        ax.set_title('Job Timeline')
        plt.show()

        # print(df)
        # fig = px.timeline(df, x_start="startTime", x_end="endTime", y="jobName", color="jobName",title=f'Gantt Chart - {strategy}')
        # fig.update_yaxes(autorange = "reversed")  # Set the order of y-axis labels
        # fig.show()

# Function to calculate and plot average metrics
def plot_metrics(data):
    avg_response_times = {}
    avg_turnaround_times = {}
    
    for strategy, values in data.items():
        if "Avg Response Time" in values and "Avg Turnaround Time" in values:
            avg_response_times[strategy] = sum(values["Avg Response Time"]) / len(values["Avg Response Time"])
            avg_turnaround_times[strategy] = sum(values["Avg Turnaround Time"]) / len(values["Avg Turnaround Time"])
    
    plt.figure(figsize=(10, 4))
    plt.title('Average Response Time Comparison')
    plt.bar(avg_response_times.keys(), avg_response_times.values(), color='g')
    plt.ylabel('Average Response Time')
    plt.ylim(min(avg_response_times.values()) - 5, max(avg_response_times.values()) + 5)
    plt.show()

    plt.figure(figsize=(10, 4))
    plt.title('Average Turnaround Time Comparison')
    plt.bar(avg_turnaround_times.keys(), avg_turnaround_times.values(), color='r')
    plt.ylabel('Average Turnaround Time')
    plt.ylim(min(avg_turnaround_times.values()) - 5, max(avg_turnaround_times.values()) + 5)
    plt.show()

if __name__ == "__main__":
    filename = "out.txt"
    data1, data2 = parse_data(filename)
    plot_gantt_chart(data1)
    plot_metrics(data2)
