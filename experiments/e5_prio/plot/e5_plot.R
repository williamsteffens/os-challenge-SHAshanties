library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

#CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\test\\experiments\\e5_prio')

data5 = read.table(".\\data\\e5_data.csv", header = TRUE, sep = ',')

data5Mean <- data5 %>% group_by(server, testtype) %>% summarise(scoreMean = mean(score))

head(data5Mean[order(data5Mean$scoreMean) , ])

#myColors = c("#DA8A67", "#7fbd60", "#008080", "#6d4196")

data5Mean$server <- factor(data5Mean$server, levels = c("noPrio", "Prio"))

data5Mean %>%
  ggplot(aes(x = factor(server), y = scoreMean, fill = factor(server))) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "Cached thread pool server vs. priority cached thread pool server",
       subtitle = "servers vs. average score",
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
ggsave(".\\plot\\prio_e5.png", width = 12, height = 7, type = "cairo")
