using HedgingEngine.GrpcClient;
using HedgingEngine.RebalancingOracle;
using MarketData;
using ParameterInfo;
using ParameterInfo.RebalancingOracleDescriptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;
using TimeHandler;

//hedger is the class that will be used to hedge the portfolio
namespace HedgingEngine.Hedger
{
    public class Hedger
    {
        public PricerClient PricerClient;
        public List<OutputData> OutputDataList;
        public List<DataFeed> HedgingPast;
        public Portfolio.Portfolio HedgingPortfolio;
        public IRebalacingOracle rebalacingOracle;
        public Hedger(DataFeed initDataFeed, PricerClient pricer)
        {
            PricerClient = pricer;
            OutputDataList = new List<OutputData>();
            HedgingPast = new List<DataFeed>();
            InitializeRebalancingOracle();
            InitializePortfolio(initDataFeed);
        }
//initalisation du portefeuille
        private void InitializePortfolio(DataFeed initDataFeed)
        {
            TestParameters testParams = PricerClient.testParameters;
            DateTime currentDate = initDataFeed.Date;
            bool monitoringDateReached = testParams.PayoffDescription.PaymentDates.Contains(currentDate);
            MathDateConverter mathDateConverter = new MathDateConverter(testParams.NumberOfDaysInOneYear);
            double time = mathDateConverter.ConvertToMathDistance(testParams.PayoffDescription.CreationDate, currentDate);
            List<DataFeed> past = fillPast(initDataFeed);
            PricerClient.ComputePricesAndDeltas(past.ToArray(), monitoringDateReached, time);  
            int nbAssets = PricerClient.deltas.Length;
            double initPrice = PricerClient.price;
            HedgingPortfolio = new Portfolio.Portfolio(initPrice, nbAssets);
            HedgingPortfolio.updateComposition(PricerClient.deltas.ToArray(), initDataFeed);
            fillOutputDataList(currentDate);

        }
        //initialisation de l'oracle de rebalancement
        private void InitializeRebalancingOracle()
        {
            IRebalancingOracleDescription description = PricerClient.testParameters.RebalancingOracleDescription;
            rebalacingOracle = new FixedTimesOracle(description);
        }
        public void Hedge(List<DataFeed> dataFeeds)
        {
            TestParameters testParams = PricerClient.testParameters;
            MathDateConverter mathDateConverter = new MathDateConverter(testParams.NumberOfDaysInOneYear);
            double riskFreeRate = testParams.AssetDescription.CurrencyRates[testParams.AssetDescription.DomesticCurrencyId];
            DateTime OldDate = dataFeeds[0].Date;
            
            foreach (DataFeed dataFeed in dataFeeds.Skip(1))
            {
                DateTime currentDate = dataFeed.Date;
                double time = mathDateConverter.ConvertToMathDistance(testParams.PayoffDescription.CreationDate, currentDate);
                bool monitoringDateReached = testParams.PayoffDescription.PaymentDates.Contains(currentDate);
                List<DataFeed> newPast = fillPast(dataFeed);
                PricerClient.ComputePricesAndDeltas(newPast.ToArray(), monitoringDateReached, time);
                double capTime = mathDateConverter.ConvertToMathDistance(OldDate, currentDate);
                HedgingPortfolio.UpdatePortfolioValue(dataFeed, capTime, riskFreeRate);
                OldDate = currentDate;
                if (rebalacingOracle.rebalancingDate(currentDate))
                {
                    HedgingPortfolio.updateComposition(PricerClient.deltas, dataFeed);
                    fillOutputDataList(currentDate);
                }
            }
        }
        private void fillOutputDataList(DateTime date)
        {
            OutputDataList.Add(new OutputData()
            {
                Date = date,
                Deltas = PricerClient.deltas,
                DeltasStdDev = PricerClient.deltasStdDev,
                Price = PricerClient.price,
                PriceStdDev = PricerClient.priceStdDev,
                Value = HedgingPortfolio.PortfolioValue
            }) ;

        }

        private List<DataFeed> fillPast(DataFeed newDataFeed)
        {
            List<DataFeed> past = new List<DataFeed>();

            foreach (DataFeed dataFeed in HedgingPast) {
                past.Add(dataFeed) ;
            }
            
            past.Add(newDataFeed);

            if (PricerClient.testParameters.PayoffDescription.PaymentDates.Contains(newDataFeed.Date))
            {
                HedgingPast.Add(newDataFeed);
            }
            return past ;
        }

    }
}
