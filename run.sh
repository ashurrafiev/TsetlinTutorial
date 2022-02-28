./mnist -step-size 10000 -steps 60 -acc-eval-test 1 -log-tastates -log-status -save-state -save-state-path state0.csv
./mnist -step-size 10000 -steps 60 -acc-eval-test 1 -log-tastates -log-status -log-append -load-state -load-state-path state0.csv -remap-state -save-state -save-state-path state1.csv
./mnist -step-size 10000 -steps 60 -acc-eval-test 1 -log-tastates -log-status -log-append -load-state -load-state-path state1.csv -remap-state -save-state -save-state-path state2.csv
