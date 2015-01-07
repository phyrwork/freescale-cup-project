% StartSession
% --------------------------
% Right now, this method needs to be a script so that if the script is
% interrupted (i.e. stopped by the user ungracefully until I figure out
% a nicer way to do it) the 'local' data isn't lost.

% if it isn't already, configure the serial connection
if (~exist('frdm'))
    frdm = SerialInit('COM3', 115200)
end

% rename the most recent session
if (exist('session'))
    evalStr = ['session_',strrep(session.id, '-', '_'),' = session;']; %replace hyphens with underscores
    eval(evalStr);
end

% set up the new session
session = TftpSession(timestampStr(), frdm)
gui = GuiSession(session);
gui = gui.addViews(...
        ...
        LinescanView(session, 'linescan0', 'figure', 1, 'position', [2,1,1]),...
        TrackPositionView(session, 'TrackPosition', 'figure', 1, 'position', [2,1,2]),...
        ...
        MotorTorqueView(session, 'T_rl', 'figure', 2, 'position', [2,4,1]),...
        MotorTorqueView(session, 'T_rr', 'figure', 2, 'position', [2,4,5]),...
        ...
        MotorTorqueView(session, 'T*_rl', 'figure', 2, 'position', [2,4,2]),...
        MotorTorqueView(session, 'T*_rr', 'figure', 2, 'position', [2,4,6]),...
        ...
        PidView(session, 'PID_T_rl', 'figure', 2, 'position', [2,4,3]),...
        PidView(session, 'PID_T_rr', 'figure', 2, 'position', [2,4,7]),...
        ...
        MotorDutyView(session, 'D_rl', 'figure', 2, 'position', [2,4,4]),...
        MotorDutyView(session, 'D_rr', 'figure', 2, 'position', [2,4,8])...
        ...
      )
        %MotorCurrentView(session, 'i_rl', 'figure', 1, 'position', [2,5,4]),...
        %MotorCurrentView(session, 'i_rr', 'figure', 1, 'position', [2,5,9]),...
% set up the timers

    % set up the serial process timer
    timers(1) = timer;
        % settings
        timers(1).ExecutionMode = 'fixedRate';
        timers(1).Period = 0.1;
        timers(1).BusyMode = 'drop';
        % callbacks
        timers(1).TimerFcn = 'session = session.receive();';
        % finish up
        timers(1) % print settings
    
    % set up the gui process timer
    timers(2) = timer;
        % settings
        timers(2).ExecutionMode = 'fixedRate';
        timers(2).Period = 0.25;
        timers(2).BusyMode = 'queue';
        % callbacks
        timers(2).TimerFcn = 'gui = gui.update();';
        %finish up
        timers(2) % print settings
 
 % start session
 for i = 1:length(timers)
     start(timers(i));
 end
    
