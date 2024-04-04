ua = readtable('Sun Mar 17 2024 ua .csv');
windup =  readtable('Sun Mar 17 2024 windup_error .csv');
lux = readtable('Sun Mar 17 2024 lux .csv');
ref = readtable('Sun Mar 17 2024 ref .csv');
pwm = readtable('Sun Mar 17 2024 pwm .csv');


figure(1); 

hold on; 
plot(lux.timestamp / 1000, lux.lux);
plot(ref.timestamp / 1000, ref.ref);

ylabel('light measured [lux]');
xlabel('Time passed [s]');

legend('lux measured', 'reference');

title('Response with anti windup active');

