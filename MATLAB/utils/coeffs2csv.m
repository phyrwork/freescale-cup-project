function [ s ] = coeffs2csv( coeffs )
%coeffs2csv Convert vector to comma separated string of values
    
    s = [];
    for i = 1:length(coeffs) - 1
       ns = num2str(coeffs(i));
       s = [s, ns, ','];
    end
    ns = num2str(coeffs(end));
    s = [s, ns];
end

