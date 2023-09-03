import subprocess

def compare_outputs(expected_output, actual_output):
    with open(expected_output, 'r') as f_expected, open(actual_output, 'r') as f_actual:
        expected_lines = f_expected.readlines()
        actual_lines = f_actual.readlines()

    return expected_lines == actual_lines

if __name__ == "__main__":
        
    # Compile the C program
    compile_process = subprocess.run(["gcc", "scheduler.c", "-o", "scheduler"])

    if compile_process.returncode == 0:
        # Run the compiled program with command line arguments
        inputFile= "./inputTestcases/inpTestcase"
        outFile="./yourOutput/outTestcase"
        resFile="./expectedOutput/results"
        for i in range(1,30):
            inp= inputFile+str(i)+".txt"
            out=outFile+str(i)+".txt"
            res=resFile+str(i)+".txt"
            args = ["./scheduler",inp,out, "6", "5", "3", "2", "8"]
            subprocess.run(args)
            if compare_outputs(out,res):
                if(i<=9):
                    print(f"Test case 0{i}: Accepted")
                else:
                    print(f"Test case {i}: Accepted")
            else:
                if(i<=9):
                    print(f"Test case 0{i}: Failed")
                else:
                    print(f"Test case {i}: Failed")
    else:
        print("Compilation failed")
        
