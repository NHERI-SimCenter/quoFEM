F=dlmread('1_4032477_FY.txt');
D=dlmread('1_4032477_EDY.txt');

figure(1); plot(D(:,2),F(:,end));
xlabel('disp');
ylabel('force');
set(gcf,'color','w'); grid on;