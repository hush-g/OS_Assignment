import matplotlib.pyplot as plt

# Read scheduling information from the file
with open("scheduling_info.txt", "r") as file:
    lines = file.readlines()

# Initialize empty lists to store data
process_ids = []
start_times = []
end_times = []

# Parse the data from each line and store it in the lists
for line in lines:
    parts = line.strip().split(":")
    process_id = int(parts[0].split()[-1])
    start_time = int(parts[1].split()[-1])
    end_time = int(parts[2].split()[-1])
    
    process_ids.append(process_id)
    start_times.append(start_time)
    end_times.append(end_time)

# Create a Gantt chart using matplotlib
plt.figure(figsize=(10, 5))
plt.barh(process_ids, [end - start for start, end in zip(start_times, end_times)], left=start_times)
plt.xlabel("Time")
plt.ylabel("Process ID")
plt.title("Gantt Chart")
plt.show()

# Calculate and print average turnaround time
turnaround_times = [end - start for start, end in zip(start_times, end_times)]
average_turnaround_time = sum(turnaround_times) / len(turnaround_times)
print(f"Average Turnaround Time: {average_turnaround_time:.2f}")

# Calculate and print average response time
response_times = [start - start_times[0] for start in start_times]
average_response_time = sum(response_times) / len(response_times)
print(f"Average Response Time: {average_response_time:.2f}")
