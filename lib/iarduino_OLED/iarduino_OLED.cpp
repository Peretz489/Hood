#include "iarduino_OLED.h"																								//
																														//
//		Инициализация дисплея:																							//	Возвращаемое значение: отсутствует
void	iarduino_OLED::begin(void){																						//	Параметр: отсутствует
//			Инициируем работу с шиной I2C:																				//
			objI2C->begin(100);																							//	Инициируем передачу данных по шине I2C на скорости 100 кГц.
//			Подготавливаем к работе дисплей:																			//	
			_sendCommand(SSD1306_DISPLAY_OFF);																			//	Выключаем дисплей.
			_sendCommand(SSD1306_SET_DISPLAY_CLOCK);	_sendCommand(0x80);												//	Устанавливаем частоту обновления дисплея в значение 0x80 (по умолчанию).
			_sendCommand(SSD1306_SET_MULTIPLEX_RATIO);	_sendCommand(0x3F);												//	Устанавливаем multiplex ratio (коэффициент мультиплексирования COM выводов) в значение 0x3F (по умолчанию).
			_sendCommand(SSD1306_SET_DISPLAY_OFFSET);	_sendCommand(0x00);												//	Устанавливаем смещение дисплея в 0 (без смещения).
			_sendCommand(SSD1306_SET_START_LINE|0);																		//	Устанавливаем смещение ОЗУ в значение 0 (без смещения).
			_sendCommand(SSD1306_CHARGE_DCDC_PUMP);		_sendCommand(0x14);												//	Настраиваем схему питания (0x14 - включить внутренний DC-DC преобразователь, 0x10 - отключить внутренний DC/DC).
			_sendCommand(SSD1306_ADDR_MODE);			_sendCommand(0x00);												//	Устанавливаем режим автоматической адресации (0x00-горизонтальная, 0x01-вертикальная, 0x10-страничная.)
			_sendCommand(SSD1306_SET_REMAP_L_TO_R);																		//	Устанавливаем режим строчной развертки (слева/направо).
			_sendCommand(SSD1306_SET_REMAP_T_TO_D);																		//	Устанавливаем режим кадровой развертки (сверху/вниз).
			_sendCommand(SSD1306_SET_COM_PINS);			_sendCommand(0x12);												//	Устанавливаем аппаратную конфигурация COM выводов в значение 0x12 (по умолчанию)
			_sendCommand(SSD1306_SET_CONTRAST);			_sendCommand(0xCF);												//	Устанавливаем контрастность в значение 0xCF (допустимы значения от 0x00 до 0xFF).
			_sendCommand(SSD1306_SET_PRECHARGE_PERIOD);	_sendCommand(0xF1);												//	Настраиваем схему DC/DC преобразователя (0xF1 - Vcc снимается с DC/DC преобразователя, 0x22 - Vcc подается извне).
			_sendCommand(SSD1306_SET_VCOM_DESELECT);	_sendCommand(0x40);												//	Устанавливаем питание светодиодов VcomH в значение выше чем по умолчанию (0x30), что увеличит яркость дисплея (допустимые значения: 0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70).
			_sendCommand(SSD1306_RAM_ON);																				//	Разрешаем отображать содержимое RAM памяти.
			_sendCommand(SSD1306_INVERT_OFF);																			//	Отключаем инверсию.
			_sendCommand(SSD1306_DISPLAY_ON);																			//	Включаем дисплей.
			clrScr();																									//	Чистим экран.
}																														//
																														//
//		Очистка дисплея:																								//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::clrScr(bool color){																				//	Параметр: флаг закрашивания экрана в белый цвет.
			memset(arrBuffer, (color?0xFF:0x00), 1024);																	//	Заполняем все биты массива arrBuffer нулями или единицами, в зависимости от состояния флага color.
			numPageS=0; numPageP=7;																						//	Указываем что при записи буфера arrBuffer в дисплей, нужно заполнить строки с 0 по 7.
			numCellS=0; numCellP=127;																					//	Указываем что при записи буфера arrBuffer в дисплей, нужно заполнить ячейки с 0 по 127.
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
}																														//
																														//
//		Инверсия цвета дисплея:																							//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::invScr(bool invert){																				//	Параметр: флаг инвертирования - да/нет.
			if(invert)	{_sendCommand(SSD1306_INVERT_ON );}																//	Если флаг invert установлен - включаем инверсию.
			else		{_sendCommand(SSD1306_INVERT_OFF);}																//	Если флаг invert сброшен    - отключаем инверсию.
}																														//
																														//
//		Выбор шрифта для вывода текста:																					//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::setFont(uint8_t* font){																			//	Параметр:  название шрифта.
			insFont.font		=	font;																				//	Сохраняем указатель на первый байт массива в области памяти программ.
			insFont.width		=	pgm_read_byte(&font[0]);															//	Сохраняем ширину символов выбранного шрифта читая её  из 0 байта массива по указателю font.
			insFont.height		=	pgm_read_byte(&font[1]);															//	Сохраняем высоту символов выбранного шрифта читая её  из 1 байта массива по указателю font.
			insFont.firstSymbol	=	pgm_read_byte(&font[2]);															//	Сохраняем код первого симола выбран. шрифта читая его из 2 байта массива по указателю font.
			insFont.sumSymbol	=	pgm_read_byte(&font[3]);															//	Сохраняем количество символов в выбр шрифте читая их  из 3 байта массива по указателю font.
			insFont.setFont		=	true;																				//	Устанавливаем флаг выбора шрифта.
			uint16_t i			=	(uint16_t) insFont.sumSymbol * insFont.width * insFont.height / 8 + 0x04;			//	Определяем позицию бита указывающего количество пустых интервалов в массиве шрифта.
			uint16_t j			=	pgm_read_byte(&font[i]);															//	Определяем количество пустых интервалов в массиве шрифта.
									insFont.startSpace[0]=0xFF;	insFont.sumSpace[0]=0;									//	Указываем что первый пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
									insFont.startSpace[1]=0xFF;	insFont.sumSpace[1]=0;									//	Указываем что второй пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
									insFont.startSpace[2]=0xFF;	insFont.sumSpace[2]=0;									//	Указываем что третий пустой интервал в массиве шрифта находится после символа с кодом (0xFF) и состоит из 0 символов
			if(j>0)				{	insFont.startSpace[0]=pgm_read_byte(&font[i+1]);									//	Если количество пустых интервалов больше 0, то	сохраняем начало первого пустого интервала символов
									insFont.sumSpace  [0]=pgm_read_byte(&font[i+2]);}									//															и размер первого пустого интервала символов
			if(j>1)				{	insFont.startSpace[1]=pgm_read_byte(&font[i+3]);									//	Если количество пустых интервалов больше 1, то	сохраняем начало второго пустого интервала символов
									insFont.sumSpace  [1]=pgm_read_byte(&font[i+4]);}									//															и размер второго пустого интервала символов
			if(j>2)				{	insFont.startSpace[2]=pgm_read_byte(&font[i+5]);									//	Если количество пустых интервалов больше 2, то	сохраняем начало третьего пустого интервала символов
									insFont.sumSpace  [2]=pgm_read_byte(&font[i+6]);}									//															и размер третьего пустого интервала символов
}																														//
																														//
//		Установка курсора в указанную позицию																			//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::setCursor(int x, int y){																			//	Параметры: № колонки, № строки.
			if(x<128){numX=x;} if(y<64){numY=y;}																		//	Сохраняем указанные позиции курсора, если они входят в диапазон допустимых
}																														//
																														//
//		Вывод текста:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(char*  txt, int x, int y){																	//	Параметры: текст, № колонки, № строки.
			_print(_codingCP866(txt), x, y);																			//	Вызываем функцию вывода текста _print().
}																														//
																														//
//		Вывод текста:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(String txt, int x, int y){																	//	Параметры: текст, № колонки, № строки.
			char i[txt.length()+1]; txt.toCharArray(i,txt.length()+1); _print(_codingCP866(i), x, y);					//	Преобразуем String в char* и вызываем функцию вывода текста _print().
}																														//
																														//
//		Вывод текста:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(const char* txt, int x, int y){															//	Параметры: текст, № колонки, № строки.
			char i[strlen(txt)+1]; for(uint8_t j=0; j<=strlen(txt); j++){i[j]=txt[j];} _print(_codingCP866(i),x,y);		//	Преобразуем const char* в char* и вызываем функцию вывода текста _print().
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(int8_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			print(int32_t(num), x, y, sys);																				//	Преобразуем int8_t в int32_t и вызываем функцию вывода числа print(int32_t).
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(uint8_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			print(uint32_t(num), x, y, sys);																			//	Преобразуем uint8_t в uint32_t и вызываем функцию вывода числа print(uint32_t).
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(int16_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			print(int32_t(num), x, y, sys);																				//	Преобразуем int16_t в int32_t и вызываем функцию вывода числа print(int32_t).
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(uint16_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			print(uint32_t(num), x, y, sys);																			//	Преобразуем uint16_t в uint32_t и вызываем функцию вывода числа print(uint32_t).
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(int32_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			int8_t i=2; int32_t j=1;			while(num/j){j*=sys; i++;}												//	Определяем количество разрядов числа (i = количество разрядов + 2, j = множитель кратный основанию системы счисления)
			char k[i]; i--; k[i]=0; i--;		if(num>0) {k[i]=0; i--;}												//	Создаём строку k из i символов и добавляем символ(ы) конца строки.
			uint32_t n=num<0?num*-1:num;		while(i)  {k[i]=_ItoAa(n%sys); n/=sys; i--;}							//	Заполняем строку k.
												if(num>=0){k[i]=_ItoAa(n%sys);}else{k[i]='-';}							//	Добавляем первый символ (либо первая цифра, либо знак минус).
			print(k, x, y);																								//	Выводим строку k.
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(uint32_t num, int x, int y, uint8_t sys){													//	Параметры: число, № колонки, № строки, система счисления.
			int8_t i=1; uint32_t j=1;			while(num/j){j*=sys; i++;} if(num==0){i++;}								//	Определяем количество разрядов числа (i = количество разрядов + 1, j = множитель кратный основанию системы счисления)
			char k[i]; i--; k[i]=0;				while(i){k[i-1]=_ItoAa(num%sys); num/=sys; i--;}						//	Определяем строку k из i символов и заполняем её.
			print(k, x, y);																								//	Выводим строку k.
}																														//
																														//
//		Вывод чисел:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::print(double num, int x, int y, uint8_t sum){													//	Параметры: число, № колонки, № строки, количество знаков после запятой
			uint32_t i=1, j=0, k=0;																						//	Определяем временные переменные.
			j=sum; while(j){i*=10; j--;} 																				//	i=10^sum.
			print(int32_t(num), x, y);																					//	Выводим целую часть числа.
			if(sum){																									//	Если требуется вывести хоть один знак после запятой, то ...
				print(".");																								//	Выводим символ разделителя.
				j=num*i*(num<0?-1:1); j%=i; k=j;																		//	Получаем целое число, которое требуется вывести после запятой.
				if(j==0){while( sum  ){print("0"); sum--;}          }													//	Если полученное целое число = 0, то выводим sum раз символ 0.
				else    {while(j*10<i){print("0"); j*=10;} print(k);}													//	Иначе, если в полученном целом числе меньше разрядов чем требуется, то заполняем эти разряды выводя символ 0, после чего выводим само число.
			}
}																														//
																														//
//		Прорисовка пикселя:																								//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawPixel(int x, int y, bool c){																	//	Параметры: № колонки, № строки, цвет.
			_drawPixel(x,y,c);																							//	Прорисовываем пиксель в буфере.
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
			numX=x; numY=y;																								//	Сохраняем координаты.
}																														//
																														//
//		Получение цвета пикселя:																						//	Возвращаемое значение: цвет (0-чёрный, 1-белый)
bool	iarduino_OLED::getPixel(int x, int y){																			//	Параметры: № колонки, № строки.
			if(x<0 || x>127 || y<0 || y>63){return 0;}																	//	Если заданы некорректные координаты, то выводим 0 (чёрный цвет пикселя).
			uint16_t numByte = ( y/8 * 128 ) + x;																		//	Определяем номер байта массива arrBuffer в котором находится пиксель.
			uint8_t  numBit  =   y%8;																					//	Определяем номер бита в найденном байте, который соответсвует искомому пикселю.
			return bitRead(arrBuffer[numByte], numBit);																	//	Возвращаем цвет пикселя из бита numBit элемента numByte массива arrBuffer.
}																														//
																														//
//		Прорисовка линии по двум точкам:																				//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawLine(int x1, int y1, int x2, int y2, bool c){												//	Параметры: № колонки начала, № строки начала, № колонки окончания, № строки окончания, цвет.
			_drawLine(x1,y1,x2,y2,c);																					//	Прорисовываем линию в буфере.
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
			numX=x2; numY=y2;																							//	Сохраняем координаты.
}																														//
																														//
//		Прорисовка линии по одной точке:																				//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawLine(int x2, int y2, bool c){																//	Параметры: № колонки окончания, № строки окончания, цвет.
			drawLine(numX,numY,x2,y2,c);																				//	Прорисовываем линию по двум точкам.
}																														//
																														//
//		Прорисовка прямоугольника по двум точкам:																		//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawRect(int x1, int y1, int x2, int y2, bool f, bool c){										//	Параметры: № колонки начала, № строки начала, № колонки окончания, № строки окончания, закрасить, цвет.
		if(f){																											//	Если прямоугольник требуется закрасить, то ...
			if(x1<x2)	{for(int x=x1; x<=x2; x++){_drawLine(x,y1,x,y2,c);}}											//	Рисуем несколко линий.
			else		{for(int x=x1; x>=x2; x--){_drawLine(x,y1,x,y2,c);}}											//	Рисуем несколко линий.
		}else{																											//	Иначе, если прямоугольник закрашивать не надо, то ...
			_drawLine(x1,y1,x2,y1,c);																					//	Прорисовываем линию.
			_drawLine(x2,y2,x2,y1,c);																					//	Прорисовываем линию.
			_drawLine(x2,y2,x1,y2,c);																					//	Прорисовываем линию.
			_drawLine(x1,y1,x1,y2,c);																					//	Прорисовываем линию.
		}																												//
		if(flgUpdate){_sendBuffer();}																					//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
		numX=x2; numY=y2;																								//	Сохраняем координаты.
}																														//
																														//
//		Прорисовка круга:																								//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawCircle(int x, int y, uint8_t r, bool f, bool c){												//	Параметры: № колонки, № строки, радиус, закрасить, цвет
			int x1=0, y1=r, p=1-r;																						//	Определяем переменные: x1,y1 - положительные координаты точек круга с центром 00. p - отрицательная парабола.
			while(x1 < y1+1){																							//	Цикл будет выполняться пока координата x не станет чуть меньше y (прочертит дугу от 0 до 45°) - это 1/8 часть круга
				if(f){																									//	Если круг требуется закрасить, то ...
					_drawLine(x-x1,y-y1,x+x1,y-y1,c);																	//	Прорисовываем горизонтальные линии вверху круга        (между точками 3 и 1 дуг, см. ниже).
					_drawLine(x-x1,y+y1,x+x1,y+y1,c);																	//	Прорисовываем горизонтальные линии внизу  круга        (между точками 4 и 2 дуг, см. ниже).
					_drawLine(x-y1,y-x1,x+y1,y-x1,c);																	//	Прорисовываем горизонтальные линии выше середины круга (между точками 7 и 5 дуг, см. ниже).
					_drawLine(x-y1,y+x1,x+y1,y+x1,c);																	//	Прорисовываем горизонтальные линии выше середины круга (между точками 8 и 6 дуг, см. ниже).
				}else{																									//	Иначе, если круг закрашивать не надо, то ...
					_drawPixel(x+x1, y-y1, c);																			//	1 дуга   0° -  45° (построенная в соответствии с уравнением)
					_drawPixel(x+x1, y+y1, c);																			//	2 дуга 180° - 135° (1 дуга отражённая по вертикали)
					_drawPixel(x-x1, y-y1, c);																			//	3 дуга 360° - 315° (1 дуга отражённая по горизонтали)
					_drawPixel(x-x1, y+y1, c);																			//	4 дуга 180° - 225° (2 дуга отражённая по горизонтали)
					_drawPixel(x+y1, y-x1, c);																			//	5 дуга  90° - 45°  (2 дуга повёрнутая на -90°)
					_drawPixel(x+y1, y+x1, c);																			//	6 дуга  90° - 135° (1 дуга повёрнутая на +90°)
					_drawPixel(x-y1, y-x1, c);																			//	7 дуга 270° - 315° (1 дуга повёрнутая на -90°)
					_drawPixel(x-y1, y+x1, c);																			//	8 дуга 270° - 225° (2 дуга повёрнутая на +90°)
				}	if(p>=0){y1--; p-=y1*2;}																			//	Если парабола p вышла в положительный диапазон, то сдвигаем её вниз на y1*2 (каждый такой сдвиг провоцирет смещение точки y1 первой дуги вниз).
					   p++;  x1++; p+=x1*2;																				//	С каждым проходом цикла, смещаем точку x1 первой дуги влево и находим новую координату параболы p.
			}																											//
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
			numX=x; numY=y;																								//	Сохраняем координаты.
}																														//
																														//
//		Прорисовка изображения:																							//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::drawImage(uint8_t* image, int x, int y, uint8_t mem){											//	Параметры: изображение, № колонки, № строки, тип памяти.
			uint8_t w = getImageWidth(image, mem);																		//	Получаем ширину изображения.
			uint8_t h = getImageHeight(image, mem);																		//	Получаем высоту изображения.
			bool c;																										//	Объявляем переменную для хранения цвета
			switch(x){																									//	Колонка с которой требуется начать вывод изображения ...
				case OLED_L: numX=0;			break;																	//	Определяем начальную колонку для выравнивания по левому краю.
				case OLED_C: numX=(128-w)/2;	break;																	//	Определяем начальную колонку для выравнивания по центру.
				case OLED_R: numX= 128-w;		break;																	//	Определяем начальную колонку для выравнивания по правому краю.
				case OLED_N: numX=numX;			break;																	//	Начальной колонкой останется та, на которой был закончен вывод предыдущего текста или изображения.
				default    : numX=x;			break;																	//	Начальная колонка определена пользователем.
			}																											//	
			switch(y){																									//	Строка с которой требуется начать вывод изображения ...
				case OLED_T: numY=h-1;			break;																	//	Определяем начальную строку для выравнивания по верхнему краю.
				case OLED_C: numY=(64-h)/2+h;	break;																	//	Определяем начальную строку для выравнивания по центру.
				case OLED_B: numY=63;			break;																	//	Определяем начальную строку для выравнивания по нижнему краю.
				case OLED_N: numY=numY;			break;																	//	Начальной строкой останется та, на которой выведен предыдущий текст или изображение.
				default    : numY=y;			break;																	//	Начальная строка определена пользователем.
			}																											//	
			for (uint8_t p=0; p<h; p++){																				//	Проходим по страницам изображения...
			for (uint8_t k=0; k<w; k++){																				//	Проходим по колонкам  изображения...
				if(mem==IMG_RAM){c=bitRead(                image[2+(p/8*w)+k],  p%8);}else								//	Если массив изображения находится в памяти ОЗУ, то получаем цвет очередного пикселя из p%8 бита, 2+(p/8*w)+k байта, массива image
				if(mem==IMG_ROM){c=bitRead( pgm_read_byte(&image[2+(p/8*w)+k]), p%8);}									//	Если массив изображения находится в памяти ПЗУ, то получаем цвет очередного пикселя из p%8 бита, 2+(p/8*w)+k байта, массива image
				if(flgImgBG || c){_drawPixel( numX+k, numY-h+1+p, c );}													//	Если у изображения есть фон, или цвет пикселя белый, то прорисовываем пиксель в координате numX+k, numY-h+1+p.
			}}	numX+=w;																								//	Добавляем ширину изображения к координате numX.
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
}																														//
																														//
//		Вывод ширины изображения:																						//	Возвращаемое значение: ширина изображения в пикселях.
uint8_t	iarduino_OLED::getImageWidth(uint8_t* image, uint8_t mem){														//	Параметры: изображение, тип памяти.
			return (mem==IMG_RAM)? image[0] : pgm_read_byte(&image[0]);													//	Возвращаем ширину изображения.
}																														//
																														//
//		Вывод высоты изображения:																						//	Возвращаемое значение: высота изображения в пикселях.
uint8_t	iarduino_OLED::getImageHeight(uint8_t* image, uint8_t mem){														//	Параметры: изображение, тип памяти.
			return (mem==IMG_RAM)? image[1] : pgm_read_byte(&image[1]);													//	Возвращаем высоту изображения.
}																														//
																														//
//		Вывод текста:																									//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_print(char* txt, int x, int y){																	//	Параметры: текст, № колонки, № строки.
			if(insFont.setFont==false || insFont.height%8>0){return;}													//	Если шрифт не выбран или его высота не кратна 8 пикселям, то выходим из функции.
			uint16_t len=strlen(txt)*insFont.width; if(len>128){len=128/insFont.width*insFont.width;}					//	Определяем количество колонок которое занимают выводимые символы.
			uint16_t num;																								//	Объявляем переменную для хранения номера байта в массиве шрифта.
			int x1, y1;																									//	Объявляем переменные для хранения координат точек.
			bool c;																										//	Объявляем переменную для хранения цвета точек.
			switch(x){																									//	Колонка с которой требуется начать вывод текста ...
				case OLED_L: numX=0;									break;											//	Определяем начальную колонку для выравнивания по левому краю.
				case OLED_C: numX=(128-len)/2;							break;											//	Определяем начальную колонку для выравнивания по центру.
				case OLED_R: numX= 128-len;								break;											//	Определяем начальную колонку для выравнивания по правому краю.
				case OLED_N: numX=numX;									break;											//	Начальной колонкой останется та, на которой был закончен вывод предыдущего текста или изображения.
				default    : numX=x;									break;											//	Начальная колонка определена пользователем.
			}																											//	
			switch(y){																									//	Строка с которой требуется начать вывод текста ...
				case OLED_T: numY=insFont.height-1;						break;											//	Определяем начальную строку для выравнивания по верхнему краю.
				case OLED_C: numY=(64-insFont.height)/2+insFont.height;	break;											//	Определяем начальную строку для выравнивания по центру.
				case OLED_B: numY=63;									break;											//	Определяем начальную строку для выравнивания по нижнему краю.
				case OLED_N: numY=numY;									break;											//	Начальной строкой останется та, на которой выведен предыдущий текст или изображение.
				default    : numY=y;									break;											//	Начальная строка определена пользователем.
			}																											//	
			if(numX+len>128){len=(128-numX)/insFont.width*insFont.width;}												//	Пересчитываем количество колонок которое занимают выводимые символы, с учётом начальной позиции.
			for(int8_t p=0; p<insFont.height/8; p++){																	//	Проходим по страницам символов...
				for(uint8_t n=0; n<(len/insFont.width); n++){															//	Проходим по выводимым символам...
					num  = uint8_t(txt[n]);																				//	Присваиваем переменной num код выводимого символа.
					if(insFont.startSpace[0]<num){num-=insFont.sumSpace[0];}											//	Если в массиве символов, до кода текущего символа, имеется пустой интервал, то уменьшаем код текущего символа на количество символов в пустом интервале.
					if(insFont.startSpace[1]<num){num-=insFont.sumSpace[1];}											//	Если в массиве символов, до кода текущего символа, имеется пустой интервал, то уменьшаем код текущего символа на количество символов в пустом интервале.
					if(insFont.startSpace[2]<num){num-=insFont.sumSpace[2];}											//	Если в массиве символов, до кода текущего символа, имеется пустой интервал, то уменьшаем код текущего символа на количество символов в пустом интервале.
					num -= insFont.firstSymbol;																			//	Вычитаем код первого символа (с которого начинается массив шрифта).
					num *= insFont.width;																				//	Умножаем полученное значение на ширину символа (количество колонок).
					num *= insFont.height/8;																			//	Умножаем полученное значение на высоту символа (количество страниц).
					num += p*insFont.width;																				//	Добавляем количество колонок данного символа, которые уже были выведены на предыдущих страницах.
					num += 0x04;																						//	Добавляем количество байт в начале массива шрифта, которые не являются байтами символов.
					for(uint8_t k=0; k<insFont.width; k++){																//	Проходим по байтам очередного символа.
						for(uint8_t b=0; b<8; b++){																		//	Проходим по байтам очередного символа.
							x1= numX + n*insFont.width + k;																//	Начальная колонка всего текста + (количество выведенных символов * ширина символов) + номер байта текущего символа.
							y1= numY - insFont.height + p*8 + b + 1;													//	Нижняя строка текста - высота симолов + количество уже выведенных страниц + номер бита байта текущего символа + 1.
							c = bitRead( pgm_read_byte(&insFont.font[num+k]), b);										//	Цвет точки символа: 1-белый, 0-чёрный.
							if(insFont.inverted){ if(insFont.background || c){_drawPixel(x1, y1, !c);}}					//	Если цвет текста    требуется инвертировать, то, если установлен фон текста или точка стоит на букве (а не на фоне), то выводим    инвертированную точку.
							else				{ if(insFont.background || c){_drawPixel(x1, y1,  c);}}					//	Если цвет текста не требуется инвертировать, то, если установлен фон текста или точка стоит на букве (а не на фоне), то выводим не инвертированную точку.
						}																								//	
					}																									//	
				}																										//	
			}																											//	
			if(flgUpdate){_sendBuffer();}																				//	Если установлен флаг flgUpdate, то отправляем массив arrBuffer на дисплей.
			numX+=len;																									//	Сохраняем координату окончания текста.
}																														//
																														//
//		Преобразование одной цифры в один символ:																		//	Возвращаемое значение: символ.
char	iarduino_OLED::_ItoAa(uint8_t num){																				//	Параметр: одна цифра от 0 до 15.
			return char( num + (num<10?48:55) );																		//	Преобразуем цифры 0-9 в символ с кодом 48-57, а цифры 10-15 в символ с кодом 65-71.
}																														//
																														//
//		Преобразование строки из кодировки UTF-8 в кодировку CP866:														//	Возвращаемое значение: строка в кодировке CP866.
char*	iarduino_OLED::_codingCP866(char* StrIn){																		//	Параметр: строка в кодировке UTF-8.
			char*	StrOut=StrIn;																						//	Определяем строку для вывода результата.
			uint8_t	numIn    =	0,																						//	Определяем переменную хранящую номер символа в строке StrIn.
					numOut   =	0,																						//	Определяем переменную хранящую номер символа в строке StrOut.
					charThis =	StrIn[0],																				//	Определяем переменную хранящую код текущего символа в строке StrIn.
					charNext =	StrIn[1];																				//	Определяем переменную хранящую код следующего символа в строке StrIn.
			switch(codingName){																							//	Тип кодировки строки StrIn.
			//	Преобразуем текст из кодировки UTF-8:																	//
				case TXT_UTF8:																							//
					while (charThis> 0   &&numIn    <0xFF                ){												//	Если код текущего символа строки StrIn больше 0 и № текушего символа строки StrIn меньше 255, то ...
						if(charThis==0xD0&&charNext>=0x90&&charNext<=0xBF){StrOut[numOut]=charNext-0x10; numIn++;}else	//	Если код текущего символа равен 208, а за ним следует символ с кодом 144...191, значит это буква «А»...«п» требующая преобразования к коду 128...175
						if(charThis==0xD0&&charNext==0x81                ){StrOut[numOut]=         0xF0; numIn++;}else	//	Если код текущего символа равен 208, а за ним следует символ с кодом 129      , значит это буква «Ё»       требующая преобразования к коду 240
						if(charThis==0xD1&&charNext>=0x80&&charNext<=0x8F){StrOut[numOut]=charNext+0x60; numIn++;}else	//	Если код текущего символа равен 209, а за ним следует символ с кодом 128...143, значит это буква «р»...«я» требующая преобразования к коду 224...239
						if(charThis==0xD1&&charNext==0x91                ){StrOut[numOut]=         0xF1; numIn++;}else	//	Если код текущего символа равен 209, а за ним следует символ с кодом 145      , значит это буква «ё»       требующая преобразования к коду 241
						                                                  {StrOut[numOut]=charThis;}     numIn++;		//	Иначе не меняем символ.
						numOut++; charThis=StrIn[numIn]; charNext=StrIn[numIn+1];										//	Переходим к следующему символу.
					}	StrOut[numOut]='\0';																			//	Добавляем символ конца строки и возвращаем строку StrOut.
				break;																									//
			//	Преобразуем текст из кодировки WINDOWS-1251:															//
				case TXT_WIN1251:																						//
					while (charThis> 0   &&numIn    <0xFF){																//	Если код текущего символа строки StrIn больше 0 и № текушего символа строки StrIn меньше 255, то ...
						if(charThis>=0xC0&&charThis<=0xEF){StrOut[numOut]=charThis-0x40;}else							//	Если код текущего символа равен 192...239, значит это буква «А»...«п» требующая преобразования к коду 128...175
						if(charThis>=0xF0&&charThis<=0xFF){StrOut[numOut]=charThis-0x10;}else							//	Если код текущего символа равен 240...255, значит это буква «р»...«я» требующая преобразования к коду 224...239
						if(charThis==0xA8                ){StrOut[numOut]=         0xF0;}else							//	Если код текущего символа равен 168      , значит это буква «Ё»       требующая преобразования к коду 240
						if(charThis==0xB8                ){StrOut[numOut]=         0xF1;}else							//	Если код текущего символа равен 184      , значит это буква «ё»       требующая преобразования к коду 241
						                                  {StrOut[numOut]=charThis;}									//	Иначе не меняем символ.
						numIn++; numOut++; charThis=StrIn[numIn];														//	Переходим к следующему символу.
					}	StrOut[numOut]='\0';																			//	Добавляем символ конца строки и возвращаем строку StrOut.
				break;																									//
			}	return StrOut;																							//	Возвращаем строку StrOut.
}																														//
																														//
//		Прорисковка пикселя в буффере:																					//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_drawPixel(int x, int y, bool c){																//	Параметры: x, y, цвет
			if(x<0 || x>127 || y<0 || y>63){return;}																	//	Если заданы некорректные координаты, то выходим из функции.
			uint8_t  p=y/8;																								//	Определяем номер страницы в которой должен находиться пиксель
			uint16_t numByte = ( p * 128 ) + x;																			//	Определяем номер байта массива arrBuffer в котором требуется прорисовать пиксель.
			uint8_t  numBit  =   y%8;																					//	Определяем номер бита в найденном байте, который соответсвует рисуемому пикселю.
			if(c)	{arrBuffer[numByte] |=   1<<numBit; }																//	Прорисовываем белую точку в буфере arrBuffer.
			else	{arrBuffer[numByte] &= ~(1<<numBit);}																//	Прорисовываем чёрную точку в буфере arrBuffer.
			if(p<numPageS){numPageS=p;}																					//	Обновляем номер первой страницы в которую требуется записать данные из буфера arrBuffer.
			if(p>numPageP){numPageP=p;}																					//	Обновляем номер последней страницы в которую требуется записать данные из буфера arrBuffer.
			if(x<numCellS){numCellS=x;}																					//	Обновляем номер первой ячейки в которую требуется записать данные из буфера arrBuffer.
			if(x>numCellP){numCellP=x;}																					//	Обновляем номер последней ячейки в которую требуется записать данные из буфера arrBuffer.
}																														//
																														//
//		Прорисковка линии в буффере:																					//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_drawLine(int x1, int y1, int x2, int y2, bool c){												//	Параметры: x1, y1, x2, y2, цвет.
			int x3 = x2-x1;																								//	
			int y3 = y2-y1;																								//	
			if(abs(x3)>abs(y3))	{	if(x1<x2)	{for(int x=x1; x<=x2; x++){_drawPixel(x, ((x-x1)*y3/x3+y1), c);}}		//	Рисуем линию по линейному уровнению (y-y1)/(y2-y1) = (x-x1)/(x2-x1)
									else		{for(int x=x1; x>=x2; x--){_drawPixel(x, ((x-x1)*y3/x3+y1), c);}}}		//	Первое условие определяет где больше расстояние (по оси x или y), по той оси о проходим в цикле, для поиска точек на другой оси
			else				{	if(y1<y2)	{for(int y=y1; y<=y2; y++){_drawPixel(((y-y1)*x3/y3+x1), y, c);}}		//	Второе условие определяет как проходить по циклу (в прямом или обратном направлении).
									else		{for(int y=y1; y>=y2; y--){_drawPixel(((y-y1)*x3/y3+x1), y, c);}}}		//	
}																														//	
																														//
//		Отправка байта команды:																							//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_sendCommand(uint8_t command){																	//	Параметр: код команды.
			objI2C->writeByte(dispAddr, SSD1306_COMMAND, command);														//	Отправляем 3 байта: адрес dispAddr с битом rw=0 (запись), SSD1306_COMMAND, command.
}																														//
																														//
//		Отправка указанного количества байтов данных:																	//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_sendData(uint8_t* data, uint8_t sum){															//	Параметры: указатель на начало массива данных, количество передаваемых байт
			if(objI2C->getType()==4){																					//	Если передача данных осуществляется под управлением библиотеки Wire.h, то ...
			for(uint8_t i=0; i<sum; i+=30){objI2C->writeBytes(dispAddr,SSD1306_DATA,&data[i],((i+30)<sum)?30:(sum-i));}}//	Передаём данные многобайтным пакетом, но не более 30 байт в одном пакете (ограничение буфера Wire.h)
			else                          {objI2C->writeBytes(dispAddr,SSD1306_DATA, data, sum);}						//	Передаём данные многобайтным пакетом не задумываясь о его размере.
}																														//
																														//
//		Отправка буфера (массива arrBuffer) в дисплей:																	//	Возвращаемое значение: отсутствует.
void	iarduino_OLED::_sendBuffer(void){																				//	Параметр: отсутствует.
			for(uint8_t p=numPageS; p<=numPageP; p++){																	//	Проходим по страницам экрана, от numPageS до numPageP включительно.
				_sendCommand(SSD1306_ADDR_PAGE);																		//	Установка адреса страницы.
				_sendCommand(p);																						//	Начало.
				_sendCommand(p);																						//	Конец.
				_sendCommand(SSD1306_ADDR_COLUMN);																		//	Установка адреса колонки.
				_sendCommand(numCellS);																					//	Начало.
				_sendCommand(numCellP);																					//	Конец.
				_sendData(&arrBuffer[(p*128+numCellS)], (numCellP-numCellS+1));											//	Заполняем все колонки очередной страницы.
			}																											//
		    numPageS=numPageP=0;																						//	Указываем что при записи буфера arrBuffer в дисплей, нужно заполнить строки с 0 по 0.
		    numCellS=numCellP=0;																						//	Указываем что при записи буфера arrBuffer в дисплей, нужно заполнить ячейки с 0 по 0.
}																														//