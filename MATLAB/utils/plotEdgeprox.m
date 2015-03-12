function [ Dmax ] = plotEdgeprox( h, vn, angf, angv, p )
%plotEdgeprox Plot estimated vision of vertical-mount linescan given
%parameters.

    for i = 1:p
        [tmp.D, tmp.ang] = edgeprox(h, vn, angf, angv, p, i-1);
        if tmp.ang >= 90
            break;
        else
            res(i) = tmp;
        end
    end
    
    y = [res(:).D];
    
    plot(1:length(y),y);
    Dmax = max(y);
    
end

