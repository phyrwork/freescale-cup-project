function [ dx ] = derivative( x )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    dx = x(2:end) - x(1:end-1);

end

