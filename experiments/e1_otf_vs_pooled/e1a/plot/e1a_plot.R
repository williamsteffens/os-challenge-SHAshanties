library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\test\\experiments\\e1_otf_vs_pooled\\e1a')

data1a = read.table(".\\data\\e1a_data.csv", header = TRUE, sep = ',')

data1aMean <- data1a %>% group_by(nprocesses) %>% summarise(scoreMean = mean(score))

head(data1aMean[order(data1aMean$scoreMean) , ])

data1aMean %>%
  ggplot(aes(x = factor(nprocesses), y = scoreMean)) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "nprocesses vs. average score",
       subtitle = "Pre-forked server",
       x = "nprocesses",
       y = "average score") +
  scale_y_continuous(breaks = scales::pretty_breaks(n = 10)) +
  theme_fivethirtyeight() + 
  theme(text = element_text(size = 14), axis.title = element_text(), legend.position = "right")

ggsave(".\\plot\\preforked_e1a.png", width = 12, height = 4.5, type = "cairo")
  
  