import csv

with open('../data/human.txt', 'w') as human_file:
    human_file.write('')
with open('../data/gpt.txt', 'w') as gpt_file:
    gpt_file.write('')

with open('../data/dataset/GPT-wiki-intro.csv', 'r') as file:
    reader = csv.reader(file)
    next(reader)
    for row in reader:
        wiki_intro = row[3]
        generated_intro = row[4]
        
        # remove newline characters, apart from the last one
        # in order to distinguish different entries
        wiki_intro = wiki_intro.replace('\n', ' ') + '\n'
        generated_intro = generated_intro.replace('\n', ' ') + '\n'

        with open('../data/human.txt', 'a') as human_file:

            human_file.write(wiki_intro)

        with open('../data/gpt.txt', 'a') as gpt_file:
            gpt_file.write(generated_intro)

        