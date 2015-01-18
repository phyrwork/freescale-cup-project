function filter = BuildGaussian( size, sigma )
%BuildGaussian Summary of this function goes here
%   Detailed explanation goes here
    x = linspace(-size/2, size/2, size);
    filter = exp(-x .^ 2/(2*sigma^2));
    filter = filter / sum(filter);
end
