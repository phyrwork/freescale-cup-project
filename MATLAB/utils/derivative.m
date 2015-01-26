function [ dx ] = derivative( x )
%derivative Find discrete derivative of a 1-d vector

    % get dims of x
    [r,c] = size(x);
    if (r == 1 && c ~= 1) && (c == 1 && r ~= 1)
        error('Vector must be 1-dimensional');
    end
    
    % find derivative
    dx = x(2:end) - x(1:end-1);
    
    % prepend a zero to sync length
    if r > c
        dx = [0; dx];
    else
        dx = [0, dx];
    end
end

