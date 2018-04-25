

#!/bin/sh

SESSION_NAME="big_red"

cd ~/Sites/within3/big_red

tmux has-session -t ${SESSION_NAME}

if [ $? != 0 ]
then
# Create the session
tmux new-session -s ${SESSION_NAME} -n vim -d

# First window (0) -- vim and console
tmux send-keys -t ${SESSION_NAME} 'vim' C-m

# shell (1)
tmux new-window -n bash -t ${SESSION_NAME}
tmux send-keys -t ${SESSION_NAME}:1 'git status' C-m

# mysql (2)
tmux new-window -n mysql -t ${SESSION_NAME}
tmux send-keys -t ${SESSION_NAME}:2 'mysql -u <username> <database>' C-m

# server/debug log (3)
tmux new-window -n server -t ${SESSION_NAME}
tmux send-keys -t ${SESSION_NAME}:3 'bundle exec rails s' C-m
tmux split-window -v -t ${SESSION_NAME}:3
tmux send-keys -t ${SESSION_NAME}:3.1 'tail -f log/development.log | grep "DEBUG"' C-m

# rails console (4)
tmux new-window -n console -t ${SESSION_NAME}
tmux send-keys -t ${SESSION_NAME}:4 'pry -r ./config/environment' C-m

# Start out on the first window when we attach
tmux select-window -t ${SESSION_NAME}:0
fi
tmux attach -t ${SESSION_NAME}
