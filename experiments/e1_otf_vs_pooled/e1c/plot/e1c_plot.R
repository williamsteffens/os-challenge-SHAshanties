library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

#CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\experiments\\e1_otf_vs_pooled\\e1c')

data1c = read.table(".\\data\\e1c_data.csv", header = TRUE, sep = ',')

data1cMean <- data1c %>% group_by(server, testtype) %>% summarise(scoreMean = mean(score))

#head(data1cMean[order(data1cMean$scoreMean) , ])

#myColors = c("#DA8A67", "#7fbd60", "#008080", "#6d4196")

data1cMean %>%
  ggplot(aes(x = factor(server), y = scoreMean, fill = factor(server))) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "4-way comparison of different multiprocessing and multithreading servers",
       subtitle = "servers vs. average score",
       x = "servers",
       y = "average score",
       fill = "Servers") +
  scale_y_continuous(breaks = scales::pretty_breaks(n = 15)) +
  theme_fivethirtyeight() + 
  theme(text = element_text(size = 14), 
        axis.title = element_text(), 
        axis.text.x = element_text(angle = 45, vjust = 1, hjust=1), 
        legend.position = "right", 
        legend.direction="vertical", 
        panel.spacing = unit(3, "lines")) +
  facet_grid(~ testtype, scales = "free")
ggsave(".\\plot\\compare_e1c.png", width = 12, height = 7, type = "cairo")
