function [ y ] = divsigmoid( x )
%derivitive of the sigmoid function
%   y=exp(-x)./((1+exp(-x)).^2)

y=exp(-x)./((1+exp(-x)).^2);
end

