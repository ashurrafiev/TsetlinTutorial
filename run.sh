# ./mnist -step-size 10000 -steps 60 -acc-eval-classes 1 -log-tastates -log-status -save-state -save-state-path state0.csv
# ./mnist -step-size 10000 -steps 60 -acc-eval-classes 1 -log-tastates -log-status -log-append -load-state -load-state-path state0.csv -remap-state -save-state -save-state-path state1.csv
# ./mnist -step-size 10000 -steps 60 -acc-eval-classes 1 -log-tastates -log-status -log-append -load-state -load-state-path state1.csv -remap-state -save-state -save-state-path state2.csv
#./mnist -step-size 2000 -steps 300 -acc-eval-classes 1 -log-tastates -log-status
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -save-state -save-state-path state0.csv
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -load-state -load-state-path state0.csv -save-state -save-state-path state1.csv
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -load-state -load-state-path state1.csv -save-state -save-state-path state2.csv
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -load-state -load-state-path state2.csv -save-state -save-state-path state3.csv
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -load-state -load-state-path state3.csv -save-state -save-state-path state4.csv
#./mnist -step-size 10000 -steps 6 -acc-eval-test 1 -load-state -load-state-path state4.csv -save-state -save-state-path state5.csv
#./mnist -step-size 10000 -steps 30 -acc-eval-test 1 -load-state -load-state-path state5.csv -save-state -save-state-path state10.csv
#./mnist -step-size 10000 -steps 30 -acc-eval-test 1 -load-state -load-state-path state10.csv -save-state -save-state-path state15.csv
./mnist -step-size 60000 -steps 5 -acc-eval-test 1
