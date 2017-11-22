#!/bin/bash
./vmsim 16 4 2 < test1Input.txt > test1Output.txt
diff test1Output.txt test1Answers.txt
./vmsim 6 3 3 < test2Input.txt > test2Output.txt
diff test2Output.txt test2Answers.txt
./vmsim 16 4 2 < test3Input.txt > test3Output.txt
diff test3Output.txt test3Answers.txt