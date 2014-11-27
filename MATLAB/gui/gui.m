function gui( session, displayPeriod )
%gui
    % plot the last displayPeriod seconds of data for attributes
    time = session.records(15).times(session.records(15).rsize);
    
    % TrackPosition
    [times, values] = session.records(15).afterTime(time - displayPeriod);
    subplot(2,1,1, 'replace')
    plot(times, values)
    
    % linescan0
    values = session.records(1).values(:,session.records(1).rsize);
    subplot(2,1,2, 'replace')
    plot(1:length(values), values);
end

