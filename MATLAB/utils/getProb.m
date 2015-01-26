function P = getProbabilty( value, mean, sigma )
%getProbabilty
    
    P = ...
    (0.39894*(1/sigma)*power(2.71828,-power(value - mean,2)/(2.0*power(sigma,2)))) / ...
    (0.39894*(1/sigma)*power(2.71828,(-power(0,2))/(2*power(sigma,2))));

end

