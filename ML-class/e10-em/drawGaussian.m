function drawGaussian(mu, cov, props)

[U, D, V] = svd(cov);
lamba = diag(D);

mu1 = mu + sqrt(lamba(1)) * V(1,:);
mu2 = mu + sqrt(lamba(2)) * V(2,:);

hold on
plot(mu(1), mu(2), 'bo')
plot([mu(1) mu1(1)], [mu(2) mu1(2)], props)
plot([mu(1) mu2(1)], [mu(2) mu2(2)], props)
hold off

end