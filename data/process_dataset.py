import csv

with open('./train/human.txt', 'w') as human_file:
    human_file.write('')
with open('./train/gpt.txt', 'w') as gpt_file:
    gpt_file.write('')

with open('./dataset/GPT-wiki-intro.csv', 'r') as file:
    reader = csv.reader(file)
    # skip the header
    next(reader)
    reader_list = list(reader)
    # split the dataset into training and testing sets (90% for training, 10% for testing)
    split_point = int(0.9 * len(reader_list))
    reader_train = reader_list[:split_point]
    reader_test = reader_list[split_point:]

    for row in reader_train:
        wiki_intro = row[3]
        generated_intro = row[4]
        
        # remove newline characters, apart from the last one
        # in order to distinguish different entries
        wiki_intro = wiki_intro.replace('\n', ' ') + '\n'
        generated_intro = generated_intro.replace('\n', ' ') + '\n'

        with open('./train/human.txt', 'a') as human_file:
            human_file.write(wiki_intro)

        with open('./train/gpt.txt', 'a') as gpt_file:
            gpt_file.write(generated_intro)

    for i, row in enumerate(reader_test):
        wiki_intro = row[3]
        generated_intro = row[4]

        wiki_intro = wiki_intro.replace('\n', ' ') + '\n'
        generated_intro = generated_intro.replace('\n', ' ') + '\n'
        
        with open('./test/human_' + str(i+1) + '.txt', 'w') as human_file:
            human_file.write(wiki_intro)
        
        with open('./test/gpt_' + str(i+1) + '.txt', 'w') as gpt_file:
            gpt_file.write(generated_intro)

        