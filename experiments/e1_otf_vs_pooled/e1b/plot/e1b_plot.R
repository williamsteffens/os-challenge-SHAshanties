library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

#CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\test\\experiments\\e1_otf_vs_pooled\\e1b')

data1b = read.table(".\\data\\e1b_data.csv", header = TRUE, sep = ',')

data1bMean <- data1b %>% group_by(nthreads) %>% summarise(scoreMean = mean(score))

head(data1bMean[order(data1bMean$scoreMean) , ])

data1bMean %>%
  ggplot(aes(x = factor(nthreads), y = scoreMean)) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "nthreads vs. average score",
       subtitle = "Thread Pool Server",
       x = "nthreads",
       y = "average score") +
  scale_y_continuous(breaks = scales::pretty_breaks(n = 10)) +
  theme_fivethirtyeight() + 
  theme(text = element_text(size = 14), axis.title = element_text(), legend.position = "right")

ggsave(".\\plot\\threadpool_e1b.png", width = 12, height = 4.5, type = "cairo")
