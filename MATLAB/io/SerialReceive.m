function [ frames ] = SerialReceive( device )
%SerialReceive

    % initialise return variable
    frames = {}; % array of cells to allow frames of varying width
    
    % if no new data, no new frames
    rsize = device.BytesAvailable;
    if (rsize == 0)
        return;
    end
    
    % define persistent read buffer for function and read from serial
    persistent buffer;
    [new, bytesRead] = fread(device,rsize);
    buffer = [buffer; new];
    
    % Calculate data rate
    persistent tval;
    try
        telap = toc(tval); % time elapsed since last read
        datarate = bytesRead/telap;
        disp(['Data rate: ',num2str(datarate),' B/s'])
    catch
    end
    tval = tic;
    
    % do
    while (true)
       % find next frame boundary
       b = find(buffer == 0, 1, 'first');
       if (isempty(b)) break; end; % if no results then more complete frames in buffer
       
       % extract frame and remove from buffer
       frame = buffer(1:b);
       frame = uint8(transpose(frame)); % vert to horz vector
       buffer = buffer(b + 1:length(buffer));
       
       % add frame to cell array of frames
       frames{length(frames) + 1} = frame;
    end
end

