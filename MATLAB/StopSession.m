% StopSession
% -------------------------
% Pause all process timers

for i = 1:length(timers)
    stop(timers(i));
end