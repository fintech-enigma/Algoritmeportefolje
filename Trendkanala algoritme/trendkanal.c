
#property copyright "Copyright 2023, MetaQuotes Ltd."
#property link      "fintechenigma.com"
#property version   "1.00"
#property indicator_chart_window
#property indicator_buffers 2
#property indicator_plots   2
#property indicator_color1 DodgerBlue
#property indicator_color2 Red

#include <Trade\PositionInfo.mqh>
#include <trade/trade.mqh>
#include <trendlinjer.mqh>
#include <Trade\Trade.mqh>

MqlTick currentTick, previousTick;

CTrade trade;
CPositionInfo position;

double iMABuffer[];
//double iMABuffer2[];
input int   InpStart    =  1;
int   InpLength   =  50;
//int   InpLength1   =;
datetime gBarTime;
datetime lastBarTime;
int Bars;


input double stopLossPips = 0.040;
input double tpPips  = 0.060;;
static input long   InpMagicnumber = 64356; // magicnumber
static input double InpLots = 0.01;    // lot size
input bool trailingStopLoss = true; // Enable trailing stop loss
input int stopLossPoints = 4000;
/* Strategi som skal implementere både brudd og retur av pris
bool sale = false;
//bool buy = false;
//int telle_buy = 0;
int telle_sell = 0;
double sell_price = 0;
*/



int OnInit()
  {
   
   trade.SetExpertMagicNumber(InpMagicnumber);
   return(INIT_SUCCEEDED);
  }



void  OnTick() {
   
   datetime rightBarTime = iTime(_Symbol, _Period, 0);
   
   //Moving averaget
   CopyBuffer(iMA(_Symbol,_Period,200,0,MODE_EMA,PRICE_CLOSE),0,0,1,iMABuffer);
   
   //CopyBuffer(iMA(_Symbol,_Period,50,0,MODE_EMA,PRICE_CLOSE),1,0,1,iMABuffer2);
   //double ima0 = iMABuffer[0];
   //Print(ima0);
   
   static int prev_slope;
   static int slopen;
   static double ima0;
   static double ima1;
   if(rightBarTime != gBarTime)
     {
      // Global variabel == riktig tid på bar
      gBarTime = rightBarTime;
      ima1 = ima0;
      ima0 = iMABuffer[0];
      
      if(ima0/*-(ima0*0.00015))*/ > ima1)
        {
         slopen = 1; // Positive slope
        }
      else
         if(ima0/*+(ima0*0.00015))*/ < ima1)
           {
            slopen = -1; // Negative slope
           }
           
      if(slopen != prev_slope)
        {
         InpLength = 10;
        }
      
       prev_slope = slopen;
       
       InpLength++;
       
      
     }
   
   

   //Trendlinjene
   CTrendHiLo  *  trend =  new CTrendHiLo(InpStart, InpLength);
   
   trend.Update();
   
   PrintFormat("Upper at %i is %f", InpStart+InpLength, trend.UpperValueAt(InpStart+InpLength));
   PrintFormat("Lower at %i is %f", InpStart+InpLength, trend.LowerValueAt(InpStart+InpLength));
   
   ObjectDelete(0, "UpperTrend");
   ObjectDelete(0, "LowerTrend");
   
   
   ObjectCreate(0, "UpperTrend", OBJ_TREND, 0,
                  iTime(Symbol(), Period(), InpStart+InpLength),
                     trend.UpperValueAt(InpStart+InpLength),
                     iTime(Symbol(), Period(), 0), trend.UpperValueAt(0));
   ObjectSetInteger(0, "UpperTrend", OBJPROP_COLOR, Red);
   ObjectSetInteger(0, "UpperTrend", OBJPROP_WIDTH, 2);
   
   
   ObjectCreate(0, "LowerTrend", OBJ_TREND, 0,
                  iTime(Symbol(), Period(), InpStart+InpLength),
                     trend.LowerValueAt(InpStart+InpLength),
                     iTime(Symbol(), Period(), 0), trend.LowerValueAt(0));
   ObjectSetInteger(0, "LowerTrend", OBJPROP_COLOR, Blue);
   ObjectSetInteger(0, "LowerTrend", OBJPROP_WIDTH, 2);
   
   
   
   
   // Ny stick, oppdater gammel pris
   if(IsNewBar()){previousTick = currentTick;}
   
   
   if(!SymbolInfoTick(_Symbol, currentTick)){Print("Failed to print current tick"); return;}
   
   // Sjekke posisjoner
   int cntBuy, cntSell;
   if(!CountOpenPositions(cntBuy, cntSell)){return;}
   
   string   objName_lower  =  "LowerTrend";
   string   objName_higher =  "UpperTrend";
   
   datetime time  =  TimeCurrent();
   
   datetime twoBBT = iTime(_Symbol, _Period, 2);
   
   
   //Finne trendlinje prisene
   double   price_lower   =  ObjectGetValueByTime(0, objName_lower, twoBBT);
   double   price_higher  =  ObjectGetValueByTime(0, objName_higher, twoBBT);
   
   
   
   // Kalkulere førstegangsstopp loss
   double stopLossPrice = currentTick.bid - stopLossPips;
   double tpPrice       = currentTick.ask + tpPips;
   
   
  
   
   
   //selg
   if(cntSell==0 && previousTick.ask<price_higher && currentTick.ask>=price_higher){
      
      trade.PositionOpen(_Symbol, ORDER_TYPE_SELL, InpLots, currentTick.bid , tpPrice, stopLossPrice, "Nils");
      
      /* Annen strategi som jobbes med
      sell_price = currentTick.ask;
      sale = true;
      telle_sell++;
      */
   }
   
   /* Jobber her med å se hvordan prisen har reagert etter ett visst antall barer etter kjøpssignal
   if (sale==true && IsNewBar()){
      telle_sell ++;
      if(telle_sell>=6){
         if (currentTick.ask>sell_price){
            trade.PositionOpen(_Symbol, ORDER_TYPE_BUY, InpLots, currentTick.ask, stopLossPrice, tpPrice, "Nils");
            }
         if (currentTick.ask<=sell_price){
            trade.PositionOpen(_Symbol, ORDER_TYPE_SELL, InpLots, currentTick.bid , tpPrice, stopLossPrice, "Nils");
            }
         }
      }
      
    */  
   
   
   // Kjøp
   if(cntBuy==0 && previousTick.bid>price_lower && currentTick.bid<=price_lower){
   
      
      
      ulong ticket_buy = trade.PositionOpen(_Symbol, ORDER_TYPE_BUY, InpLots, currentTick.ask, stopLossPrice, tpPrice, "Nils");
      
     
      
   
   }
   
   // Trailing stop loss 
   double stopLoss = stopLossPoints * Point();
   uint PositionsCount = PositionsTotal();
   if(trailingStopLoss && PositionsCount > 0)
     {
      for(int i = PositionsCount-1; i >= 0; i--)
        {
         if(position.SelectByIndex(i) && position.Symbol() == Symbol())
           {
            ENUM_POSITION_TYPE type = position.PositionType();
            double CurrentSL = position.StopLoss();
            double CurrentTP = position.TakeProfit();
            double CurrentPrice = position.PriceCurrent();
    
            if(type == POSITION_TYPE_BUY)
              {
               if(CurrentPrice - stopLoss > CurrentSL || CurrentSL == 0.0)
                 {
                  trade.PositionModify(position.Ticket(), NormalizeDouble((CurrentPrice - stopLoss), Digits()), 0);
                 }
              }
            if(type == POSITION_TYPE_SELL)
              {
               if(CurrentPrice + stopLoss < CurrentSL || CurrentSL == 0.0)
                 {
                  trade.PositionModify(position.Ticket(), NormalizeDouble((CurrentPrice + stopLoss), Digits()), 0);
                 }
              }
           }
        }
     }
   
   delete trend;
   

}

//Sjekke for ny bar
bool IsNewBar(){

   static datetime previousTime = 0;
   datetime currentTime = iTime(_Symbol, PERIOD_CURRENT, 0);
   if(previousTime!=currentTime) {
      previousTime=currentTime;
      return true;
   }
   return false;

}




// Telle åpne posisjoner
bool CountOpenPositions(int &cntBuy, int &cntSell){
   
   cntBuy = 0;
   cntSell = 0;
   int total = PositionsTotal();
   for(int i=total-1; i>=0; i--){
      ulong ticket = PositionGetTicket(i);
      if(ticket<=0){Print("Failed to get position ticket"); return false;}
      if(!PositionSelectByTicket(ticket)){Print("Failed to select position"); return false;}
      long magic;
      if(!PositionGetInteger(POSITION_MAGIC, magic)){Print("Failed to get magicnumber"); return false;}
      if(magic==InpMagicnumber){
         long type;
         if(!PositionGetInteger(POSITION_TYPE, type)){Print("Failed to get position again"); return false;}
         if(type==POSITION_TYPE_BUY){cntBuy++;}
         if(type==POSITION_TYPE_SELL){cntSell++;}
      }
   }
   
   return true;
}