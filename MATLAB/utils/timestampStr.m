function [ output ] = timestampStr ()
%timestampStr Return a string containing the current date and time
    
    % get date and time as a row vector
    n = clock();
    %(1) year
    %(2) month
    %(3) day
    %(4) hour
    %(5) minutes
    %(6) seconds
    
    % convert numbers to strings
    n(6) = floor(n(6)); % floor seconds first
    % concatenate
    output = char([]);
    for i = 1:length(n)
        c = num2str(n(i));
        output = [output, c];
        
        % conditionally hyphenate
        switch (i)
            % add hyphens between date elements
            case 1
            case 2
                output = [output, '-'];
            % add a double hyphen between date and time
            case 3
                output = [output, '--'];
            case 5    
                output = [output, '-'];
            otherwise
        end
            
    end
end


