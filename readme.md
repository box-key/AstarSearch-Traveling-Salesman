## Requirement

Your data must follow the following format:
- The first line only includes the number of nodes in your graph.
- You should use a complete undirected graph.
- The first column and the second column specifies two undirected graphs conncted with each other, and the third column specifies the weight of the path.

Please refer to sample_data.txt for the detail.

## Usage

1. This program requires two arguments: path to input file and output file.
2. Once you specify the argument, run the program.
3. Enter the start node and specifies a heuristic function from three choices:
  1) Calculate the average of the weights of the rest of unvisited nodes
  2) Calcuate the median of the weights of the rest of unvisited nodes
4. Open output file to check the answer.
