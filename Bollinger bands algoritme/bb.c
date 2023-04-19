#property copyright "2023, Sondre Nilsen"
#property link      "fintechenigma.com"
#property version   "1.00"
#property strict
#property indicator_chart_window
#property indicator_buffers 6
#property indicator_color1 Green
#property indicator_color2 Red
#property indicator_color3 DodgerBlue
#property indicator_color4 Orange
#property indicator_color5 Blue
#property indicator_color6 Purple

double BuyArrow[];
double SellArrow[];
double BBUpper[]; 
double BBMiddle[];
double BBLower[];
double MA[];
int telleVariabel;

int OnInit()
{
   IndicatorBuffers(6);
   IndicatorDigits(Digits);
   
   SetIndexStyle(0, DRAW_ARROW, STYLE_SOLID, 2);
   SetIndexArrow(0, 225);
   SetIndexBuffer(0, BuyArrow);
   
   SetIndexStyle(1, DRAW_ARROW, STYLE_SOLID, 2);
   SetIndexArrow(1, 226);
   SetIndexBuffer(1, SellArrow);
   
   SetIndexStyle(2, DRAW_LINE, STYLE_SOLID, 1);
   SetIndexBuffer(2, BBUpper);
   
   SetIndexStyle(3, DRAW_LINE, STYLE_SOLID, 1);
   SetIndexBuffer(3, BBMiddle);

   SetIndexStyle(4, DRAW_LINE, STYLE_SOLID, 1);
   SetIndexBuffer(4, BBLower);
   
   SetIndexStyle(5, DRAW_LINE, STYLE_SOLID, 1);
   SetIndexBuffer(5, MA);

   return(INIT_SUCCEEDED);
}

int OnCalculate(const int rates_total, const int prev_calculated, const datetime &time[], const double &open[], const double &high[], const double &low[], const double &close[], const long &tick_volume[], const long &volume[], const int &spread[])
{
   int bbPeriod = 20;
   int bbDeviations = 2;
   double bbStdDev;

   ArraySetAsSeries(close, true);
   ArraySetAsSeries(BBUpper, true);
   ArraySetAsSeries(BBMiddle, true);
   ArraySetAsSeries(BBLower, true);
   ArraySetAsSeries(MA, true);
   ArraySetAsSeries(BuyArrow, true);
   ArraySetAsSeries(SellArrow, true);

   int start = prev_calculated - 1;
   if (start < bbPeriod)
      start = bbPeriod;

   for (int i = start; i < rates_total; i++)
   {
      BBMiddle[i] = iMA(NULL, 0, bbPeriod, 0, MODE_SMA, PRICE_CLOSE, i);
      
      MA[i] = iMA(NULL, 0, 200, 0, MODE_EMA, PRICE_CLOSE, i);
      
      bbStdDev = iStdDev(NULL, 0, bbPeriod, 0, MODE_SMA, PRICE_CLOSE, i);

      BBUpper[i] = BBMiddle[i] + bbDeviations * bbStdDev;
      BBLower[i] = BBMiddle[i] - bbDeviations * bbStdDev;

      if (i > 0)
      {
         if (telleVariabel>30 && low[i] <= BBLower[i] && low[i - 1] > BBLower[i - 1]&& MA[i-1]>MA[i])
         {
            BuyArrow[i] = low[i] - (600 * Point);
            SellArrow[i] = EMPTY_VALUE;
            telleVariabel = 0;
         }
         else if (telleVariabel>30 && high[i] >= BBUpper[i] && high[i - 1] < BBUpper[i - 1] && MA[i-1]<MA[i])
         {
            SellArrow[i] = high[i] + (500 * Point);
            BuyArrow[i] = EMPTY_VALUE;
            telleVariabel = 0;
         }
         else
         {
            BuyArrow[i] = EMPTY_VALUE;
            SellArrow[i] = EMPTY_VALUE;
            telleVariabel++;
            
         }
      }
   }

   return(rates_total);
}