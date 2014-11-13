classdef TftpModule
    %TftpModule
    
    properties
        ptype = cast([], 'char');   % module type - attribute/command?
        code  = cast([], 'uint8');  % unique attribute code
        fsize = cast([], 'uint16'); % data size in frame in bytes
    end
end

