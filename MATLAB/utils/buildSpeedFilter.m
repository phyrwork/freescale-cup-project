function [ coeffs ] = buildSpeedFilter( size, sigma, epr )
%buildSpeedFilter

    % generate coefficients
    g = BuildGaussian(size, sigma);
    
    % find contribution intervals
    sum = 0;
    for i = 1:length(g)
        sum = sum + g(i);
        if (sum > 0.70)
            cs = i;
            break;
        end
    end
    
    disp('70% contribution delay:')
    
    rps = 100/60; %revs per second
    eps = rps * epr; %events per second
    dly = 1000*(cs/eps); %delay (ms)
    disp([num2str(dly),'ms @100RPM']);
    
    rps = 300/60; %revs per second
    eps = rps * epr; %events per second
    dly = 1000*(cs/eps); %delay (ms)
    disp([num2str(dly),'ms @300RPM']);
    
    rps = 600/60; %revs per second
    eps = rps * epr; %events per second
    dly = 1000*(cs/eps); %delay (ms)
    disp([num2str(dly),'ms @600RPM']);
    
    rps = 1000/60; %revs per second
    eps = rps * epr; %events per second
    dly = 1000*(cs/eps); %delay (ms)
    disp([num2str(dly),'ms @1000RPM']);
    
    figure;
    plot(g);
    
    coeffs = coeffs2csv(g);
end

