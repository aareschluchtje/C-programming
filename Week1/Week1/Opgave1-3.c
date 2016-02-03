#include <stdio.h>
	void main()
	{
		float fahr, celsius;
		int laagste, hoogste, interval;
		laagste = 0;
		hoogste = 300;
		interval = 20;

		fahr = laagste;
		printf("temperatuurconversieprogramma:n");
		while( fahr <= hoogste)
		{
			celsius = (5.0 / 9.0) * (fahr - 32.0);
			printf("%3.0f %6.1f\n", fahr, celsius);
			fahr = fahr + interval;
		}

	}