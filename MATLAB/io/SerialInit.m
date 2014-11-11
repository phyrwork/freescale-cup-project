function [ device ] = SerialInit( port, baud )
%SerialInit Set up serial port

    % configure serial port
    try 
        device = serial(port); % create object
    catch
        error(['Error initialising serial device on ', port]);
    end
    set(device, 'BaudRate', baud); % set baud rate
    set(device, 'InputBufferSize', 2^16); % set input buffer to 1MB
    fopen(device); % connect to device
end

