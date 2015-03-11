function D = edgeprox( h, vn, angf, angv, p, n )
%proximity Model of corner proximity sensor using a sideways mounted
%linescan camera. Returns distance from front of vehicle for a feature at a
%particular pixel for the given parameters.

    angf = deg2rad(angf);
    angv = deg2rad(angv);
    
    D = tan( angf - (angv/2) + (n+1)*( angv/( (2*p)+1) ) ) - vn;

end

