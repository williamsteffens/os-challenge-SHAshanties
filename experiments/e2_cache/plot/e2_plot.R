library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

#CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\test\\experiments\\e2_cache')

data2 = read.table(".\\data\\e2_data.csv", header = TRUE, sep = ',')

data2Mean <- data2 %>% group_by(server, testtype) %>% summarise(scoreMean = mean(score))

head(data1cMean[order(data1cMean$scoreMean) , ])

#myColors = c("#DA8A67", "#7fbd60", "#008080", "#6d4196")

data2Mean$server <- factor(data2Mean$server, levels = c("noCache", "Cache"))

data2Mean %>%
  ggplot(aes(x = factor(server), y = scoreMean, fill = factor(server))) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "servers vs. average score",
       subtitle = "thread pool server vs. cached thread pool server",
       x = "servers",
       y = "average score",
       fill = "Servers") +
  scale_y_continuous(breaks = scales::pretty_breaks(n = 13)) +
  theme_fivethirtyeight() + 
  theme(text = element_text(size = 14), 
        axis.title = element_text(), 
        axis.text.x = element_text(angle = 45, vjust = 1, hjust=1), 
        legend.position = "right", 
        legend.direction="vertical", 
        panel.spacing = unit(2, "lines")) +
  facet_grid(~ testtype, scales = "free")
ggsave(".\\plot\\cached_vs_thread_e2.png", width = 12, height = 7, type = "cairo")
