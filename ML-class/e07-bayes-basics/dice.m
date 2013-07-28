D1 = [1/6 1/6 1/6 1/6 1/6 1/6];
%D2 = [1/6 1/6 1/6 1/6 1/6 1/6];
%D3 = [1/6 1/6 1/6 1/6 1/6 1/6];
D2 = [0.1 0.3 0.1 0.1 0.3 0.1];
D3 = [0.4 0.2 0.1 0.1 0.1 0.1];

PSD1 = zeros(16,6);

% fill P(S,D1)
for i = 1:6
  for j = 1:6
    for k = 1:6
      PSD1(i+j+k-2,i) += D1(i)*D2(j)*D3(k);
    end
  end
end


PS_D1 = zeros(16,6);
for i = 1:6
  PD1(i) = sum(PSD1(:,i)); % P(D1)
  for s = 1:16
    PS_D1(s,i) = PSD1(s,i) / PD1(i); %(P(S|D1)
  end
end

PS_D1