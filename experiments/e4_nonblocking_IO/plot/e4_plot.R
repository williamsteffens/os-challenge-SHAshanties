library(ggplot2)
library(ggthemes)
library(dplyr)
library(Cairo)

#CairoWin()

setwd('X:\\main-folder\\O\\OS\\os-challenge-SHAshanties\\experiments\\e4_nonblocking_IO')

data4 = read.table(".\\data\\e4_data.csv", header = TRUE, sep = ',')

data4Mean <- data4 %>% group_by(server, testtype) %>% summarise(scoreMean = mean(score))

head(data4Mean[order(data4Mean$scoreMean) , ])

#myColors = c("#DA8A67", "#7fbd60", "#008080", "#6d4196")

data4Mean$server <- factor(data4Mean$server, levels = c("Cache", "NonblockingNoSplit", "NonblockingSplit"))

data4Mean %>%
  ggplot(aes(x = factor(server), y = scoreMean, fill = factor(server))) + 
  geom_bar(stat = "identity", position = "dodge", width = 0.5, alpha = 0.9) + 
  labs(title = "3-way comparison of cached thread pool and nonblocking I/O w/ and w/o split requests",
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
ggsave(".\\plot\\nonblock_e4.png", width = 14, height = 7, type = "cairo")
